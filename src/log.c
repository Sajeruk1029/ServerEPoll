/**
	*	@file
	*	@brief Структуры и функции для работы с логированием. Реализация.
	*	@author Кузнецов Егор Ильич goga.petrovich12@gmail.com
	*	@version 1.0.0
	*	@date 29.11.2025
*/

//----------------------------------------------------------------
//  Автор
//----------------------------------------------------------------
//    Кузнецов Егор Ильич
//    goga.petrovich12@gmail.com
//----------------------------------------------------------------
//	29.11.2025
//----------------------------------------------------------------
//  Заголовки
//----------------------------------------------------------------
//		C собственные заголовки
//----------------------------------------------------------------
#include	<log.h>
//----------------------------------------------------------------
//	Функции
//----------------------------------------------------------------
	int32_t	print
					(
						DebugLevel	level,
						FILE*	dst,
						const	char*	restrict	tag,
						const	char*	restrict	message
					)
	{
		if(!message)
		{
			errno	=	EINVAL;

			return	-1;
		}

		switch(level)
		{
			case	ASSERT:{ return	printAssert(stderr, message); }
			case	ERROR:{ return	printError(stderr, message); }
			case	WARN:{ return	printWarn(stderr, message); }
			case	DEBUG:{ return	printDebug(stdout, message); }
			case	VERBOSE:{ return	printVerbose(stdout, message); }
			case	INFO:{ return	printInfo(stdout, message); }
			case	TAG:{ return	printTag(dst, tag, message); }
			case	ERRNO:{ return	printErrnoError(stderr); }
			default:
			{
				errno	=	EINVAL;

				return	-1;
			}
		}
	}
	int32_t	printTag
					(
						FILE*	dst,
						const	char*	restrict	tag,
						const	char*	restrict	message
					)
	{
		errno	=	EINVAL;

		if(!dst){ return	-1; }
		if(!message){ return	-1; }

		errno	=	0;

		if(tag){ fprintf(dst, "%s:%i - %s: %s\n", __FILE__, __LINE__, tag, message); }
		else{ fprintf(dst, "%s:%i - %s\n", __FILE__, __LINE__, message); }

		return	0;
	}
	int32_t	printAssert(FILE*	dst, const	char*	restrict	message)
	{ return	printTag(dst, "ASSERT", message); }
	int32_t	printDebug(FILE*	dst, const	char*	restrict	message)
	{ return	printTag(dst, "DEBUG", message); }
	int32_t	printError(FILE*	dst, const	char*	restrict	message)
	{ return	printTag(dst, "ERROR", message); }
	int32_t	printInfo(FILE*	dst, const	char*	restrict	message)
	{ return	printTag(dst, "INFO", message); }
	int32_t	printVerbose(FILE*	dst, const	char*	restrict	message)
	{ return	printTag(dst, "VERBOSE", message); }
	int32_t	printWarn(FILE*	dst, const	char*	restrict	message)
	{ return	printTag(dst, "WARN", message); }
	int32_t	printErrnoError(FILE*	dst)
	{ return	printTag(dst, 0x00, strerror(errno)); }
//----------------------------------------------------------------
