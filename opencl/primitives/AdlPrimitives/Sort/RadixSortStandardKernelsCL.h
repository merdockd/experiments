static const char* radixSortStandardKernelsCL= \
"/*\n"
"Bullet Continuous Collision Detection and Physics Library\n"
"Copyright (c) 2011 Advanced Micro Devices, Inc.  http://bulletphysics.org\n"
"\n"
"This software is provided 'as-is', without any express or implied warranty.\n"
"In no event will the authors be held liable for any damages arising from the use of this software.\n"
"Permission is granted to anyone to use this software for any purpose, \n"
"including commercial applications, and to alter it and redistribute it freely, \n"
"subject to the following restrictions:\n"
"\n"
"1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n"
"2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n"
"3. This notice may not be removed or altered from any source distribution.\n"
"*/\n"
"//Author Takahiro Harada\n"
"\n"
"#pragma OPENCL EXTENSION cl_amd_printf : enable\n"
"#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable\n"
"\n"
"typedef unsigned int u32;\n"
"#define GET_GROUP_IDX get_group_id(0)\n"
"#define GET_LOCAL_IDX get_local_id(0)\n"
"#define GET_GLOBAL_IDX get_global_id(0)\n"
"#define GET_GROUP_SIZE get_local_size(0)\n"
"#define GROUP_LDS_BARRIER barrier(CLK_LOCAL_MEM_FENCE)\n"
"#define GROUP_MEM_FENCE mem_fence(CLK_LOCAL_MEM_FENCE)\n"
"#define AtomInc(x) atom_inc(&(x))\n"
"#define AtomInc1(x, out) out = atom_inc(&(x))\n"
"\n"
"#define make_uint4 (uint4)\n"
"#define make_uint2 (uint2)\n"
"\n"
"#define SELECT_UINT4( b, a, condition ) select( b,a,condition )\n"
"\n"
"#define WG_SIZE 128\n"
"#define NUM_PER_WI 4\n"
"\n"
"\n"
"typedef struct\n"
"{\n"
"	u32 m_key; \n"
"	u32 m_value;\n"
"}SortData;\n"
"\n"
"\n"
"typedef struct\n"
"{\n"
"	u32 m_startBit;\n"
"	u32 m_numGroups;\n"
"	u32 m_padding[2];\n"
"} ConstBuffer;\n"
"\n"
"#define BITS_PER_PASS 4\n"
"\n"
"\n"
"\n"
"uint4 prefixScanVector( uint4 data )\n"
"{\n"
"	data.y += data.x;\n"
"	data.w += data.z;\n"
"	data.z += data.y;\n"
"	data.w += data.y;\n"
"	return data;\n"
"}\n"
"\n"
"uint prefixScanVectorEx( uint4* data )\n"
"{\n"
"	uint4 backup = data[0];\n"
"	data[0].y += data[0].x;\n"
"	data[0].w += data[0].z;\n"
"	data[0].z += data[0].y;\n"
"	data[0].w += data[0].y;\n"
"	uint sum = data[0].w;\n"
"	*data -= backup;\n"
"	return sum;\n"
"}\n"
"\n"
"uint4 localPrefixSum128V( uint4 pData, uint lIdx, uint* totalSum, __local u32 sorterSharedMemory[] )\n"
"{\n"
"	{	//	Set data\n"
"		sorterSharedMemory[lIdx] = 0;\n"
"		sorterSharedMemory[lIdx+WG_SIZE] = prefixScanVectorEx( &pData );\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	{	//	Prefix sum\n"
"		int idx = 2*lIdx + (WG_SIZE+1);\n"
"		if( lIdx < 64 )\n"
"		{\n"
"			sorterSharedMemory[idx] += sorterSharedMemory[idx-1];\n"
"			GROUP_MEM_FENCE;\n"
"			sorterSharedMemory[idx] += sorterSharedMemory[idx-2];					\n"
"			GROUP_MEM_FENCE;\n"
"			sorterSharedMemory[idx] += sorterSharedMemory[idx-4];\n"
"			GROUP_MEM_FENCE;\n"
"			sorterSharedMemory[idx] += sorterSharedMemory[idx-8];\n"
"			GROUP_MEM_FENCE;\n"
"			sorterSharedMemory[idx] += sorterSharedMemory[idx-16];\n"
"			GROUP_MEM_FENCE;\n"
"			sorterSharedMemory[idx] += sorterSharedMemory[idx-32];		\n"
"			GROUP_MEM_FENCE;\n"
"			sorterSharedMemory[idx] += sorterSharedMemory[idx-64];\n"
"			GROUP_MEM_FENCE;\n"
"\n"
"			sorterSharedMemory[idx-1] += sorterSharedMemory[idx-2];\n"
"			GROUP_MEM_FENCE;\n"
"		}\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"	*totalSum = sorterSharedMemory[WG_SIZE*2-1];\n"
"	uint addValue = sorterSharedMemory[lIdx+127];\n"
"	return pData + make_uint4(addValue, addValue, addValue, addValue);\n"
"}\n"
"\n"
"\n"
"void generateHistogram(u32 lIdx, u32 wgIdx, \n"
"		uint4 sortedData,\n"
"		__local u32 *histogram)\n"
"{\n"
"    if( lIdx < (1<<BITS_PER_PASS) )\n"
"    {\n"
"    	histogram[lIdx] = 0;\n"
"    }\n"
"\n"
"	int mask = ((1<<BITS_PER_PASS)-1);\n"
"	uint4 keys = make_uint4( (sortedData.x)&mask, (sortedData.y)&mask, (sortedData.z)&mask, (sortedData.w)&mask );\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"	\n"
"	AtomInc( histogram[keys.x] );\n"
"	AtomInc( histogram[keys.y] );\n"
"	AtomInc( histogram[keys.z] );\n"
"	AtomInc( histogram[keys.w] );\n"
"}\n"
"\n"
"//\n"
"//\n"
"//\n"
"\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(WG_SIZE,1,1)))\n"
"void LocalSortKernel(__global SortData* sortDataIn, \n"
"						__global u32* ldsHistogramOut0,\n"
"						__global u32* ldsHistogramOut1,\n"
"						ConstBuffer cb)\n"
"{\n"
"\n"
"	__local u32 ldsSortData[ WG_SIZE*NUM_PER_WI + 16 ];\n"
"\n"
"	int nElemsPerWG = WG_SIZE*NUM_PER_WI;\n"
"	u32 lIdx = GET_LOCAL_IDX;\n"
"	u32 wgIdx = GET_GROUP_IDX;\n"
"	u32 wgSize = GET_GROUP_SIZE;\n"
"\n"
"    uint4 localAddr = make_uint4(lIdx*4+0,lIdx*4+1,lIdx*4+2,lIdx*4+3);\n"
"\n"
"\n"
"	SortData sortData[NUM_PER_WI];\n"
"\n"
"	{\n"
"		u32 offset = nElemsPerWG*wgIdx;\n"
"		sortData[0] = sortDataIn[offset+localAddr.x];\n"
"		sortData[1] = sortDataIn[offset+localAddr.y];\n"
"		sortData[2] = sortDataIn[offset+localAddr.z];\n"
"		sortData[3] = sortDataIn[offset+localAddr.w];\n"
"	}\n"
"\n"
"	int bitIdx = cb.m_startBit;\n"
"	do\n"
"	{\n"
"//	what is this?\n"
"//		if( lIdx == wgSize-1 ) ldsSortData[256] = sortData[3].m_key;\n"
"		u32 mask = (1<<bitIdx);\n"
"		uint4 cmpResult = make_uint4( sortData[0].m_key & mask, sortData[1].m_key & mask, sortData[2].m_key & mask, sortData[3].m_key & mask );\n"
"		uint4 prefixSum = SELECT_UINT4( make_uint4(1,1,1,1), make_uint4(0,0,0,0), cmpResult != make_uint4(0,0,0,0) );\n"
"		u32 total;\n"
"		prefixSum = localPrefixSum128V( prefixSum, lIdx, &total, ldsSortData );\n"
"\n"
"		{\n"
"			uint4 dstAddr = localAddr - prefixSum + make_uint4( total, total, total, total );\n"
"			dstAddr = SELECT_UINT4( prefixSum, dstAddr, cmpResult != make_uint4(0, 0, 0, 0) );\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			ldsSortData[dstAddr.x] = sortData[0].m_key;\n"
"			ldsSortData[dstAddr.y] = sortData[1].m_key;\n"
"			ldsSortData[dstAddr.z] = sortData[2].m_key;\n"
"			ldsSortData[dstAddr.w] = sortData[3].m_key;\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			sortData[0].m_key = ldsSortData[localAddr.x];\n"
"			sortData[1].m_key = ldsSortData[localAddr.y];\n"
"			sortData[2].m_key = ldsSortData[localAddr.z];\n"
"			sortData[3].m_key = ldsSortData[localAddr.w];\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			ldsSortData[dstAddr.x] = sortData[0].m_value;\n"
"			ldsSortData[dstAddr.y] = sortData[1].m_value;\n"
"			ldsSortData[dstAddr.z] = sortData[2].m_value;\n"
"			ldsSortData[dstAddr.w] = sortData[3].m_value;\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"\n"
"			sortData[0].m_value = ldsSortData[localAddr.x];\n"
"			sortData[1].m_value = ldsSortData[localAddr.y];\n"
"			sortData[2].m_value = ldsSortData[localAddr.z];\n"
"			sortData[3].m_value = ldsSortData[localAddr.w];\n"
"\n"
"			GROUP_LDS_BARRIER;\n"
"		}\n"
"		bitIdx ++;\n"
"	}\n"
"	while( bitIdx <(cb.m_startBit+BITS_PER_PASS) );\n"
"\n"
"	{	//	generate historgram\n"
"		uint4 localKeys = make_uint4( sortData[0].m_key>>cb.m_startBit, sortData[1].m_key>>cb.m_startBit, \n"
"			sortData[2].m_key>>cb.m_startBit, sortData[3].m_key>>cb.m_startBit );\n"
"\n"
"		generateHistogram( lIdx, wgIdx, localKeys, ldsSortData );\n"
"\n"
"		GROUP_LDS_BARRIER;\n"
"\n"
"		int nBins = (1<<BITS_PER_PASS);\n"
"		if( lIdx < nBins )\n"
"		{\n"
"     		u32 histValues = ldsSortData[lIdx];\n"
"\n"
"     		u32 globalAddresses = nBins*wgIdx + lIdx;\n"
"     		u32 globalAddressesRadixMajor = cb.m_numGroups*lIdx + wgIdx;\n"
"		\n"
"     		ldsHistogramOut0[globalAddressesRadixMajor] = histValues;\n"
"     		ldsHistogramOut1[globalAddresses] = histValues;\n"
"		}\n"
"	}\n"
"\n"
"\n"
"	{	//	write\n"
"		u32 offset = nElemsPerWG*wgIdx;\n"
"		uint4 dstAddr = make_uint4(offset+localAddr.x, offset+localAddr.y, offset+localAddr.z, offset+localAddr.w );\n"
"\n"
"		sortDataIn[ dstAddr.x + 0 ] = sortData[0];\n"
"		sortDataIn[ dstAddr.x + 1 ] = sortData[1];\n"
"		sortDataIn[ dstAddr.x + 2 ] = sortData[2];\n"
"		sortDataIn[ dstAddr.x + 3 ] = sortData[3];\n"
"	}\n"
"}\n"
"\n"
"\n"
"\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(WG_SIZE,1,1)))\n"
"void ScatterKernel(__global SortData *src,\n"
"		__global u32 *histogramGlobalRadixMajor,\n"
"		__global u32 *histogramLocalGroupMajor,\n"
"		__global SortData *dst,\n"
"		ConstBuffer cb)\n"
"{\n"
"	__local u32 sorterLocalMemory[3*(1<<BITS_PER_PASS)];\n"
"	__local u32 *ldsLocalHistogram = sorterLocalMemory + (1<<BITS_PER_PASS);\n"
"	__local u32 *ldsGlobalHistogram = sorterLocalMemory;\n"
"\n"
"\n"
"	u32 lIdx = GET_LOCAL_IDX;\n"
"	u32 wgIdx = GET_GROUP_IDX;\n"
"	u32 ldsOffset = (1<<BITS_PER_PASS);\n"
"\n"
"	//	load and prefix scan local histogram\n"
"	if( lIdx < ((1<<BITS_PER_PASS)/2) )\n"
"	{\n"
"		uint2 myIdx = make_uint2(lIdx, lIdx+8);\n"
"\n"
"		ldsLocalHistogram[ldsOffset+myIdx.x] = histogramLocalGroupMajor[(1<<BITS_PER_PASS)*wgIdx + myIdx.x];\n"
"		ldsLocalHistogram[ldsOffset+myIdx.y] = histogramLocalGroupMajor[(1<<BITS_PER_PASS)*wgIdx + myIdx.y];\n"
"		ldsLocalHistogram[ldsOffset+myIdx.x-(1<<BITS_PER_PASS)] = 0;\n"
"		ldsLocalHistogram[ldsOffset+myIdx.y-(1<<BITS_PER_PASS)] = 0;\n"
"\n"
"		int idx = ldsOffset+2*lIdx;\n"
"		ldsLocalHistogram[idx] += ldsLocalHistogram[idx-1];\n"
"		GROUP_MEM_FENCE;\n"
"		ldsLocalHistogram[idx] += ldsLocalHistogram[idx-2];\n"
"		GROUP_MEM_FENCE;\n"
"		ldsLocalHistogram[idx] += ldsLocalHistogram[idx-4];\n"
"		GROUP_MEM_FENCE;\n"
"		ldsLocalHistogram[idx] += ldsLocalHistogram[idx-8];\n"
"		GROUP_MEM_FENCE;\n"
"\n"
"		// Propagate intermediate values through\n"
"		ldsLocalHistogram[idx-1] += ldsLocalHistogram[idx-2];\n"
"		GROUP_MEM_FENCE;\n"
"\n"
"		// Grab and propagate for whole WG - loading the - 1 value\n"
"		uint2 localValues;\n"
"		localValues.x = ldsLocalHistogram[ldsOffset+myIdx.x-1];\n"
"		localValues.y = ldsLocalHistogram[ldsOffset+myIdx.y-1];\n"
"\n"
"		ldsLocalHistogram[myIdx.x] = localValues.x;\n"
"		ldsLocalHistogram[myIdx.y] = localValues.y;\n"
"\n"
"\n"
"		ldsGlobalHistogram[myIdx.x] = histogramGlobalRadixMajor[cb.m_numGroups*myIdx.x + wgIdx];\n"
"		ldsGlobalHistogram[myIdx.y] = histogramGlobalRadixMajor[cb.m_numGroups*myIdx.y + wgIdx];\n"
"	}\n"
"\n"
"	GROUP_LDS_BARRIER;\n"
"\n"
"    uint4 localAddr = make_uint4(lIdx*4+0,lIdx*4+1,lIdx*4+2,lIdx*4+3);\n"
"\n"
"	SortData sortData[4];\n"
"	{\n"
"	    uint4 globalAddr = wgIdx*WG_SIZE*NUM_PER_WI + localAddr;\n"
"		sortData[0] = src[globalAddr.x];\n"
"		sortData[1] = src[globalAddr.y];\n"
"		sortData[2] = src[globalAddr.z];\n"
"		sortData[3] = src[globalAddr.w];\n"
"	}\n"
"\n"
"	uint cmpValue = ((1<<BITS_PER_PASS)-1);\n"
"	uint4 radix = make_uint4( (sortData[0].m_key>>cb.m_startBit)&cmpValue, (sortData[1].m_key>>cb.m_startBit)&cmpValue, \n"
"		(sortData[2].m_key>>cb.m_startBit)&cmpValue, (sortData[3].m_key>>cb.m_startBit)&cmpValue );;\n"
"\n"
"	//	data is already sorted. So simply subtract local prefix sum\n"
"	uint4 dstAddr;\n"
"	dstAddr.x = ldsGlobalHistogram[radix.x] + (localAddr.x - ldsLocalHistogram[radix.x]);\n"
"	dstAddr.y = ldsGlobalHistogram[radix.y] + (localAddr.y - ldsLocalHistogram[radix.y]);\n"
"	dstAddr.z = ldsGlobalHistogram[radix.z] + (localAddr.z - ldsLocalHistogram[radix.z]);\n"
"	dstAddr.w = ldsGlobalHistogram[radix.w] + (localAddr.w - ldsLocalHistogram[radix.w]);\n"
"\n"
"	dst[dstAddr.x] = sortData[0];\n"
"	dst[dstAddr.y] = sortData[1];\n"
"	dst[dstAddr.z] = sortData[2];\n"
"	dst[dstAddr.w] = sortData[3];\n"
"}\n"
"\n"
"__kernel\n"
"__attribute__((reqd_work_group_size(WG_SIZE,1,1)))\n"
"void CopyKernel(__global SortData *src, __global SortData *dst)\n"
"{\n"
"	dst[ GET_GLOBAL_IDX ] = src[ GET_GLOBAL_IDX ];\n"
"}\n"
;