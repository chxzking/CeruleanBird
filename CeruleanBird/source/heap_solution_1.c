#include "heap_solution_1.h"

#include "EngineFault.h"	//引擎错误处理

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1                 	//条件编译判断

#define	FREE_MEMORY 				0x0						//空闲内存
#define	USAGE_MEMORY				0x1						//被使用内存
#define MINI_MEMORY_THRESHOLD 		4						//最小内存块阈值，当内存块小于这个阈值时，与前或后内存块发生合并


/**
*	@brief 内存块结构体
*
*
*  高位 attribute字段						低位（方案2）
*	+---------------------+-----------+-----+
*	|  0   1   2   3   4  |  5  |  6  |  7  | 
*	+---------------------+-----+-----+-----+
*	|    合并size长度     | 保留 | 合并| 空闲| 
*	| MERGE_FIELDS_SIZE   |     | EN  | flag|
*	+---------------------+-----+-----+-----+
*/
typedef struct MemoryBlock {
    unsigned int size;
    struct MemoryBlock* next;
    unsigned char attribute;
	
	unsigned char checksum;		//内存控制块完整性校验码（用于确认此字段是内存控制块）
}MemoryBlock;

#define MERGE_FIELDS_SIZE		5							//合并内存的位长度，(见结构体图，最长不超过6)

#define	FREE_FIELDS_MARK		0x01						//内存块空闲字段掩码
#define MERGE_FIELDS_MARK		0x02						//合并使能字段掩码
#define MERGE_SIZE_FIELDS_MARK	(0xff<<( 8 - MERGE_FIELDS_SIZE)) 	//合并字段的长度掩码

#define	MERGE_FIELDS_ENABLE		0x02						//合并标志使能
#define MERGE_FIELDS_DISNABLE	0x00						//合并标志失能



/**
*	@brief	操作合并标志位状态函数族。这个标志用于判断当前内存块是否合并了一个最小阈值的内存块
*	@param 	x-->内存块的attribute字段
*			status --> 合并标志位，可选参数：MERGE_FIELDS_ENABLE和MERGE_FIELDS_DISNABLE
*
*	@retval	GetMergeAttrStatus(x)返回值为MERGE_FIELDS_ENABLE或MERGE_FIELDS_DISNABLE
*/
//获取合并标志位的状态
#define GetMergeAttrStatus(x)	((x) & MERGE_FIELDS_MARK)		
//设置合并标志位的状态
#define SetMergeAttrStatus(x, status) ((status == MERGE_FIELDS_ENABLE) ? ((x) |= MERGE_FIELDS_ENABLE) : ((x) &= (~MERGE_FIELDS_ENABLE)))

/**
*	@brief	操作内存使用状态函数族
*	@param 	x-->内存块的attribute字段 
*			status --> 内存状态，可选参数：FREE_MEMORY和USAGE_MEMORY
*
*	@retval	GetMemoryAttrStatus(x)返回值为FREE_MEMORY和USAGE_MEMORY
*/
//获取内存块使用状态
#define GetMemoryAttrStatus(x)	((x) & FREE_FIELDS_MARK)		
//设置内存块使用状态	
#define SetMemoryAttrStatus(x,status) ( (status == FREE_MEMORY)?((x) &= (~USAGE_MEMORY)):((x) |= USAGE_MEMORY) )
/**
*	@brief	操作合并长度函数族，小于最小阈值的内存块当前是多长？
*	@param 	x-->内存块的attribute字段 
*			size --> 内存长度，可设置范围0~(2^MERGE_FIELDS_SIZE)
*
*	@retval GetMergeAttrSize(x)	返回值为size大小
*/
#define GetMergeAttrSize(x)		(((x) & MERGE_SIZE_FIELDS_MARK) >> (8 - MERGE_FIELDS_SIZE))		//获取合并内存长度
//设置合并内存长度
#define SetMergeAttrSize(x, size) ((x) = ((x) & (~MERGE_SIZE_FIELDS_MARK)) | (((size) << (8 - MERGE_FIELDS_SIZE)) & MERGE_SIZE_FIELDS_MARK))

/**
*	@brief	配置内存块属性
*	@param 	x-->内存块的attribute字段 
*			mergeStatus	-->		合并状态
*			useStatus	-->		使用状态
*			size --> 内存大小，可设置范围0~(2^MERGE_FIELDS_SIZE)
*/
#define SetMemoryAttribute(x,mergeStatus,useStatus,size)	do {\
															SetMergeAttrStatus(x, mergeStatus);\
															SetMemoryAttrStatus(x,useStatus);\
															SetMergeAttrSize(x,size);\
															} while (0)

static unsigned char MEMORY_POOL[HEAP_POOL_SIZE];			//配置内存池

unsigned char	Init_flag = 0;								//内存池初始化标志

static MemoryBlock* BlockListHeader = (MemoryBlock*)MEMORY_POOL;	//首个内存空块地址


//内存校验
// 计算 CRC8 校验码（使用 CRC-8-CCITT 多项式 0x07） 
unsigned char calculate_crc8(const unsigned char *data, unsigned int length); 
// 验证内存块的校验码 
int verifyBlock(MemoryBlock* block);															
#define VERIFYBLOCK_FAIL				0		//验证内存校验码失败
#define VERIFYBLOCK_SUCCESS				1		//验证内存校验码成功														
															
															
/******			具有内存合法性验证的新版本				*******/
//内存初始化
void memory_pool_init(void) {
	if(Init_flag == 1)	return;
	Init_flag = 1;//激活初始化标志
    BlockListHeader->size = HEAP_POOL_SIZE - sizeof(MemoryBlock);
    BlockListHeader->next = C_NULL;
	//初始化内存属性为合并失能，空闲内存和0长度
	SetMemoryAttribute(BlockListHeader->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
	// 计算校验码，将 size、next 和 attribute 字段关联
	BlockListHeader->checksum = calculate_crc8((unsigned char*)BlockListHeader, sizeof(BlockListHeader->size) + sizeof(BlockListHeader->next) + sizeof(BlockListHeader->attribute));
	return;
}
void* c_malloc(unsigned int size){
	if(size == 0)	return C_NULL;//校验输入
	//获取第一个空闲区块
	MemoryBlock* current = BlockListHeader;
	
    while (current != C_NULL) {
		//如果该区块空闲并且内存空间足够
        if ((GetMemoryAttrStatus(current->attribute) == FREE_MEMORY) && (current->size >= size)) {
			//检查此内存块被使用后剩下的空间是否小于最小空间阈值
			
			//剩下内存块不小于最小阈值，将分离出来成为新的内存块
			int Residual_memory_block = current->size - size - sizeof(MemoryBlock);
			if( Residual_memory_block >= MINI_MEMORY_THRESHOLD){
				//设置新的空闲内存块
                MemoryBlock* new_block = (MemoryBlock*)((unsigned char*)current + sizeof(MemoryBlock) + size);
                new_block->size = current->size - size - sizeof(MemoryBlock);
                new_block->next = current->next;
				SetMemoryAttribute(new_block->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
                current->size = size;
                current->next = new_block;
				//关闭当前合并标志
				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_DISNABLE);
				
			}
			//剩下内存块小于最小阈值，将作为后备内存被使用
			else{
				//合并使能
				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_ENABLE); 
				//设置为并入的块的长度
				SetMergeAttrSize(current->attribute, Residual_memory_block + sizeof(MemoryBlock));
			}
			//设置当前内存块的性质
			SetMemoryAttrStatus(current->attribute,USAGE_MEMORY);
			
			//创建内存块核验码
			current->checksum = calculate_crc8((unsigned char*)current, sizeof(current->size) + sizeof(current->next) + sizeof(current->attribute));
            //返回地址
			return (unsigned char*)current + sizeof(MemoryBlock);
        }
		//继续选择下一块
        current = current->next;
    }
    return C_NULL;  // 无足够内存
}


void* c_realloc(void* ptr,unsigned int size){
	//传入参数校验
	if(ptr == C_NULL)	return C_NULL;
	//标准化当前指针
	MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
	//检测传入的指针是否合法
	if(verifyBlock(block) == VERIFYBLOCK_FAIL){
		//【错误】<非法访问>
		EngineFault_IllegalAccess_Handle();
	}
	
	//检查是否有优化合并的操作
	//如果存在合并操作，那么则存在隐藏内存
	if(GetMergeAttrStatus(block->attribute) == MERGE_FIELDS_ENABLE){
		//检查是否为需求内存是否为内存空间
		if(size < (block->size - GetMergeAttrSize(block->attribute))){
			return C_NULL;//如果是缩小内存则立即退出
		}
		
		//检查当前内存块中隐藏的内存块是否满足需求
		int temp = block->size - size; 
		if(temp >= 0){
			//如果满足要求，则直接扩展
			SetMergeAttrSize(block->attribute, temp);
			if(temp == 0){
				//如果所有的隐藏内存刚好都被用完，则失能合并标志
				SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
			}
			//创建内存块核验码
			block->checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute));
			return ptr;//返回内存地址
		}	
	}
	else{
		//如果没有合并操作，并且新分配空间小于当前空间则视为错误操作立刻退出
		if(size < block->size)
			return C_NULL;
	}
	
	//检查下一个区块是否是空闲的
	if(block->next != C_NULL && (GetMemoryAttrStatus(block->next->attribute) == FREE_MEMORY)){
		//如果下一个区块处于空闲状态，接下来验证空间是否可以用来合并
		int temp = block->next->size - (size - block->size);//获取下一个内存块被使用后还会剩余多少空间
		
		if(temp >= MINI_MEMORY_THRESHOLD){
			//下一块空间充足
			//获取偏移量
			unsigned int offset = size - block->size;
			//获取下一个空闲内存地址
			MemoryBlock* next_block = block->next;
			//更新内存块大小
			next_block->size -= offset;
			//获取尾块地址
			next_block++;
			unsigned char* tail = ((unsigned char*)next_block);
			tail--;//回退一个字节的地址。
			
			//内存复制
			for(unsigned int i = 0;i<sizeof(MemoryBlock);i++){
				*(tail + offset) = *tail;//将该地址的值往高地址偏移offest个单位
				tail--;
			}
			unsigned char* newBlockHead = tail+offset;//获取新的内存块首地址位置
			//由于在内存复制中多减了一位，所以应该加回来
			newBlockHead++;
			
			//更新当前块next指针
			block->next = (MemoryBlock*)newBlockHead;
			//设置内存块的新大小
			block->size = size;
			//如果存在隐藏内存块，那么也被覆盖了，所以不管存不存在统一重置合并标志位失能
			SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
			//创建内存块核验码
			block->checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute));
			//返回地址
			return ptr;
		}
		//如果剩余内存块小于了阈值,但是空间足够，则直接合并
		else if(temp < MINI_MEMORY_THRESHOLD && temp >= 0){
			block->next = block->next->next;//更新next指针
			SetMergeAttrSize(block->attribute, temp + sizeof(MemoryBlock));//设置剩余数
			block->size += (block->next->size + sizeof(MemoryBlock));//设置当前内存总数
			//创建内存块核验码
			block->checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute));
			//返回地址
			return ptr;
		}
		//下一个内存块也不足够，需要分配新内存,所以继续执行下方代码
	}

	//下一块空间不充足，那么重新开辟一个新空间，并复制数据过去
	unsigned char* new_block =  (unsigned char*)c_malloc(size);
	if(new_block == C_NULL)	return C_NULL;//空间分配失败，返回空地址
		
	//空间分配成功，进行复制
	for(unsigned int i = 0;i < block->size;i++){
		new_block[i] = ((unsigned char*)ptr)[i];
	}
	//释放原空间
	c_free(ptr);
	//新建内存不需要创建内存核验码，因为c_malloc内部已经创建
	return new_block;
}


void c_free(void* ptr){
	if (ptr == C_NULL) return;//空内存返回

	//内存控制块获取
    MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
	
	//核验内存控制块释放是否合法
	if(verifyBlock(block) == VERIFYBLOCK_FAIL){
		//验证失败
		//【错误】<动态内存非法释放>
		EngineFault_IllegalFree_Handle();	
	}

	//设置该内存为空闲状态
	SetMemoryAttrStatus(block->attribute,FREE_MEMORY);

    // 以当前首个空地址内存块开始，合并相邻的空闲块
    MemoryBlock* current = BlockListHeader;
    while (current != C_NULL) {
        if ( (GetMemoryAttrStatus(current->attribute) == FREE_MEMORY ) && ( current->next != C_NULL ) && (GetMemoryAttrStatus(current->next->attribute) == FREE_MEMORY)) {
            current->size += current->next->size + sizeof(MemoryBlock);
            current->next = current->next->next;
			/*
				return 为新添加代码：考虑，由于主要在单线程环境，一次最多只可能出现一个内存块的释放，
				没有必要进行整个内存的全局扫描。
			*/
			return;
        }
        current = current->next;
    }
	
	return;
}



//内存完整性校验


// 计算 CRC8 校验码（使用 CRC-8-CCITT 多项式 0x07） 
unsigned char calculate_crc8(const unsigned char *data, unsigned int length) { 
	unsigned char crc = 0x00; 
	unsigned char poly = 0x07; // CRC-8-CCITT 多项式 
	for (unsigned int i = 0; i < length; i++) { 
		crc ^= data[i]; 
			for (unsigned char j = 0; j < 8; j++) { 
				if (crc & 0x80)
					crc = (crc << 1) ^ poly;
				else crc <<= 1;
			}
	}
	return crc; 
}

// 验证内存块的校验码,核验失败返回0，验证通过返回1
int verifyBlock(MemoryBlock* block) { 
	unsigned char expected_checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute)); 
	if(block->checksum != expected_checksum){
		return VERIFYBLOCK_FAIL;
	}
	return VERIFYBLOCK_SUCCESS;
}


#endif	//HEAP_SCHEME == HEAP_SOLUTION_1


/****************************    	以下为初始版本的代码			***********************************/
/*
	情况说明：
		1、c_malloc 经过海量测试，没有发现问题
		2、c_realloc 通过少量测试，没有发现问题
		3、c_free	没有问题
*/

////内存初始化
//void memory_pool_init(void) {
//	if(Init_flag == 1)	return;
//	Init_flag = 1;//激活初始化标志
//    BlockListHeader->size = HEAP_POOL_SIZE - sizeof(MemoryBlock);
//    BlockListHeader->next = C_NULL;
//	//初始化内存属性为合并失能，空闲内存和0长度
//	SetMemoryAttribute(BlockListHeader->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
//	return;
//}
//void* c_malloc(unsigned int size){
//	if(size == 0)	return C_NULL;//校验输入
//	//获取第一个空闲区块
//	MemoryBlock* current = BlockListHeader;
//	
//    while (current != C_NULL) {
//		//如果该区块空闲并且内存空间足够
//        if ((GetMemoryAttrStatus(current->attribute) == FREE_MEMORY) && (current->size >= size)) {
//			//检查此内存块被使用后剩下的空间是否小于最小空间阈值
//			
//			//剩下内存块不小于最小阈值，将分离出来成为新的内存块
//			int Residual_memory_block = current->size - size - sizeof(MemoryBlock);
//			if( Residual_memory_block >= MINI_MEMORY_THRESHOLD){
//				//设置新的空闲内存块
//                MemoryBlock* new_block = (MemoryBlock*)((unsigned char*)current + sizeof(MemoryBlock) + size);
//                new_block->size = current->size - size - sizeof(MemoryBlock);
//                new_block->next = current->next;
//				SetMemoryAttribute(new_block->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
//                current->size = size;
//                current->next = new_block;
//				//关闭当前合并标志
//				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_DISNABLE);
//				
//			}
//			//剩下内存块小于最小阈值，将作为后备内存被使用
//			else{
//				//合并使能
//				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_ENABLE); 
//				//设置为并入的块的长度
//				SetMergeAttrSize(current->attribute, Residual_memory_block + sizeof(MemoryBlock));
//			}
//			//设置当前内存块的性质
//			SetMemoryAttrStatus(current->attribute,USAGE_MEMORY);
//			//返回地址
//            return (unsigned char*)current + sizeof(MemoryBlock);
//        }
//		//继续选择下一块
//        current = current->next;
//    }
//    return C_NULL;  // 无足够内存
//}


//void* c_realloc(void* ptr,unsigned int size){
//	//传入参数校验
//	if(ptr == C_NULL)	return C_NULL;
//	//标准化当前指针
//	MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
//	//检查是否有优化合并的操作
//	//如果存在合并操作，那么则存在隐藏内存
//	if(GetMergeAttrStatus(block->attribute) == MERGE_FIELDS_ENABLE){
//		//检查是否为需求内存是否为内存空间
//		if(size < (block->size - GetMergeAttrSize(block->attribute))){
//			return C_NULL;//如果是缩小内存则立即退出
//		}
//		
//		//检查当前内存块中隐藏的内存块是否满足需求
//		int temp = block->size - size; 
//		if(temp >= 0){
//			//如果满足要求，则直接扩展
//			SetMergeAttrSize(block->attribute, temp);
//			if(temp == 0){
//				//如果所有的隐藏内存刚好都被用完，则失能合并标志
//				SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
//			}
//			return ptr;//返回内存地址
//		}	
//	}
//	else{
//		//如果没有合并操作，并且新分配空间小于当前空间则视为错误操作立刻退出
//		if(size < block->size)
//			return C_NULL;
//	}
//	
//	//检查下一个区块是否是空闲的
//	if(block->next != C_NULL && (GetMemoryAttrStatus(block->next->attribute) == FREE_MEMORY)){
//		//如果下一个区块处于空闲状态，接下来验证空间是否可以用来合并
//		int temp = block->next->size - (size - block->size);//获取下一个内存块被使用后还会剩余多少空间
//		
//		if(temp >= MINI_MEMORY_THRESHOLD){
//			//下一块空间充足
//			//获取偏移量
//			unsigned int offset = size - block->size;
//			//获取下一个空闲内存地址
//			MemoryBlock* next_block = block->next;
//			//更新内存块大小
//			next_block->size -= offset;
//			//获取尾块地址
//			next_block++;
//			unsigned char* tail = ((unsigned char*)next_block);
//			tail--;//回退一个字节的地址。
//			
//			//内存复制
//			for(unsigned int i = 0;i<sizeof(MemoryBlock);i++){
//				*(tail + offset) = *tail;//将该地址的值往高地址偏移offest个单位
//				tail--;
//			}
//			unsigned char* newBlockHead = tail+offset;//获取新的内存块首地址位置
//			//由于在内存复制中多减了一位，所以应该加回来
//			newBlockHead++;
//			
//			//更新当前块next指针
//			block->next = (MemoryBlock*)newBlockHead;
//			//设置内存块的新大小
//			block->size = size;
//			//如果存在隐藏内存块，那么也被覆盖了，所以不管存不存在统一重置合并标志位失能
//			SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
//			//返回地址
//			return ptr;
//		}
//		//如果剩余内存块小于了阈值,但是空间足够，则直接合并
//		else if(temp < MINI_MEMORY_THRESHOLD && temp >= 0){
//			block->next = block->next->next;//更新next指针
//			SetMergeAttrSize(block->attribute, temp + sizeof(MemoryBlock));//设置剩余数
//			block->size += (block->next->size + sizeof(MemoryBlock));//设置当前内存总数
//			//返回地址
//			return ptr;
//		}
//		//下一个内存块也不足够，需要分配新内存,所以继续执行下方代码
//	}

//	//下一块空间不充足，那么重新开辟一个新空间，并复制数据过去
//	unsigned char* new_block =  (unsigned char*)c_malloc(size);
//	if(new_block == C_NULL)	return C_NULL;//空间分配失败，返回空地址
//		
//	//空间分配成功，进行复制
//	for(unsigned int i = 0;i < block->size;i++){
//		new_block[i] = ((unsigned char*)ptr)[i];
//	}
//	//释放原空间
//	c_free(ptr);
//	return new_block;
//}


//void c_free(void* ptr){
//	if (ptr == C_NULL) return;//空内存返回

//	//内存控制块获取
//    MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
//	
//	//设置该内存为空闲状态
//	SetMemoryAttrStatus(block->attribute,FREE_MEMORY);

//    // 以当前首个空地址内存块开始，合并相邻的空闲块
//    MemoryBlock* current = BlockListHeader;
//    while (current != C_NULL) {
//        if ( (GetMemoryAttrStatus(current->attribute) == FREE_MEMORY ) && ( current->next != C_NULL ) && (GetMemoryAttrStatus(current->next->attribute) == FREE_MEMORY)) {
//            current->size += current->next->size + sizeof(MemoryBlock);
//            current->next = current->next->next;
//        }
//        current = current->next;
//    }
//	return;
//}


