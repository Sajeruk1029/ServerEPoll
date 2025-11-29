/**
	*	@file
	*	@brief Структуры и функции для работы с логированием. Заголовок.
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
#ifndef	LOG_H
#define	LOG_H
//----------------------------------------------------------------
//  Заголовки
//----------------------------------------------------------------
//		C Заголовки
//----------------------------------------------------------------
	#include	<stdio.h>
	#include	<string.h>
	#include	<errno.h>
	#include	<stdint.h>
//----------------------------------------------------------------
//	Структуры данных
//----------------------------------------------------------------
		/*	@brief	Уровень логирования. */
		typedef	enum
		{
			ASSERT,
			DEBUG,
			ERROR,
			INFO,
			VERBOSE,
			WARN,
			ERRNO,
			TAG
		}	DebugLevel;
//----------------------------------------------------------------
//	Функции
//----------------------------------------------------------------
			/**
				*	@brief	Печатать сообщение.
				*	@details	Печатает сообщение с уровнем логирования level в поток dst с тегом tag с текстом message. Следует иметь ввиду, что сообщения на уровнях отладки ASSERT, ERROR, ERRNO и WARN игнорируют dst и выводятся в stderr. ERRNO игнорирует tag. Сообщения на уровнях отладки DEBUG, INFO и VERBOSE игнорируют dst и выводятся в stdout.
				*
				*	@param[in]	level	Уровень логирования.
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	tag	Текст тега.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	print
							(
								DebugLevel	level,
								FILE*	dst,
								const	char*	restrict	tag,
								const	char*	restrict	message
							);
			/**
				*	@brief	Печатать сообщение с тегом.
				*	@details	Печатает сообщение в поток dst с тегом tag с текстом message.
				*
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	tag	Текст тега.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printTag
							(
								FILE*	dst,
								const	char*	restrict	tag,
								const	char*	restrict	message
							);
			/**
				*	@brief	Печатать ASSERT сообщение.
				*	@details	Печатает ASSERT сообщение в поток dst с текстом message.
				*
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printAssert(FILE*	dst, const	char*	restrict	message);
			/**
				*	@brief	Печатать DEBUG сообщение.
				*	@details	Печатает DEBUG сообщение в поток dst с текстом message.
				*
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printDebug(FILE*	dst, const	char*	restrict	message);
			/**
				*	@brief	Печатать ERROR сообщение.
				*	@details	Печатает ERROR сообщение в поток dst с текстом message.
				*
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printError(FILE*	dst, const	char*	restrict	message);
			/**
				*	@brief	Печатать INFO сообщение.
				*	@details	Печатает INFO сообщение в поток dst с текстом message.
				*
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printInfo(FILE*	dst, const	char*	restrict	message);
			/**
				*	@brief	Печатать VERBOSE сообщение.
				*	@details	Печатает VERBOSE сообщение в поток dst с текстом message.
				*
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printVerbose(FILE*	dst, const	char*	restrict	message);
			/**
				*	@brief	Печатать WARN сообщение.
				*	@details	Печатает WARN сообщение в поток dst с текстом message.
				*
				*	@param[in]	dst	Поток вывода.
				*	@param[in]	message	Текст сообшения.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printWarn(FILE*	dst, const	char*	restrict	message);
			/**
				*	@brief	Печатать отладочное сообщение.
				*	@details	Печатает отладочное сообщение в поток dst в формате FILE:LINE - strerror(errno).
				*
				*	@param[in]	dst	Поток вывода.
				*
				*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
			*/
			int32_t	printErrnoError(FILE*	dst);
//----------------------------------------------------------------

#endif
