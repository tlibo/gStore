/*
 * @Author: your name
 * @Date: 2022-03-28 09:14:43
 * @LastEditTime: 2022-03-28 09:14:43
 * @LastEditors: your name
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: /gstore/Util/Latch.cpp
 */
#include "Latch.h"

Latch::Latch()
// Constructor
{
	pthread_rwlock_init(&lock,0);
}
//---------------------------------------------------------------------------
Latch::~Latch()
// Destructor
{
	pthread_rwlock_destroy(&lock);
}
//---------------------------------------------------------------------------
void Latch::lockExclusive()
// Lock exclusive
{
	pthread_rwlock_wrlock(&lock);
}
//---------------------------------------------------------------------------
bool Latch::tryLockExclusive()
// Try to lock exclusive
{
	return pthread_rwlock_trywrlock(&lock)==0;
}
//---------------------------------------------------------------------------
void Latch::lockShared()
// Lock shared
{
	pthread_rwlock_rdlock(&lock);
}
//---------------------------------------------------------------------------
bool Latch::tryLockShared()
// Try to lock stared
{
	return pthread_rwlock_tryrdlock(&lock)==0;
}
//---------------------------------------------------------------------------
bool Latch::unlock()
// Release the lock
{
	return pthread_rwlock_unlock(&lock)==0;
}
