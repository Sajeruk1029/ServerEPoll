/**
	*	@file
	*	@brief Структуры и функции для работы с сообщениями. Реализация.
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
#include	<Message.h>
//----------------------------------------------------------------
//	Функции
//----------------------------------------------------------------
		int32_t	messageText(Message*	restrict	message, const	char*	restrict	text)
		{
			errno	=	EINVAL;

			if(!message){ return	-1; }
			if(!text){ return	-1; }

			errno	=	0;

			message->size	=	strlen(message->content);

			strncpy(message->content, text, MESSAGESIZE - 1);

			return	0;
		}
		int32_t	messageTime(Message*	restrict	message, const	time_t	time)
		{
			struct tm*	timeStruct	=	0x00;

			if(!message)
			{
				errno	=	EINVAL;

				return	-1;
			}

			timeStruct	=	localtime(&time);

			strftime(message->content, MESSAGESIZE - 1, "%Y-%m-%d %H:%M:%S", timeStruct);

			message->size	=	strlen(message->content);

			return	0;
		}
		int32_t	messageNetToHost(Message*	restrict	message)
		{
			if(!message)
			{
				errno	=	EINVAL;

				return	-1;
			}

			message->size	=	ntohs(message->size);

			return	0;
		}
		int32_t	messageHostToNet(Message*	restrict	message)
		{
			if(!message)
			{
				errno	=	EINVAL;

				return	-1;
			}

			message->size	=	htons(message->size);

			return	0;
		}
		int32_t	messageClear(Message*	restrict	message)
		{
			if(!message)
			{
				errno	=	EINVAL;

				return	-1;
			}

			memset(message, 0, sizeof(Message));

			return	0;
		}
		int32_t	messageStats
						(
							Message*	restrict	message,
							const	uint16_t	connections,
							const	uint16_t	currentConnections
						)
		{
			if(!message)
			{
				errno	=	EINVAL;

				return	-1;
			}

			sprintf
			(
				message->content,
				"%s: %hu %s: %hu",
				"Connections",
				connections,
				"Current Connections",
				currentConnections
			);

			message->size	=	strlen(message->content);

			return	0;
		}
		int32_t	readMessage
						(
							int32_t	sock,
							Message*	restrict	message,
							Client*	restrict	client
						)
		{
				static	int32_t	result	=	0;
	
				errno	=	EINVAL;
	
				if(!sock){ return	-1; }
				if(!message){ return	-1; }
				if(!client){ return	-1; }
	
				errno	=	0;

				client->lenInfo	=	sizeof(struct sockaddr_in);
	
				result	=	recvfrom
									(
										sock,
										&message->size,
										sizeof(uint16_t),
										MSG_WAITALL | MSG_NOSIGNAL,
										(struct sockaddr*)&client->info,
										&client->lenInfo
									);
				if(result != sizeof(uint16_t)){ return	-1; }
				result	=	recvfrom
									(
										sock,
										message->content,
										ntohs(message->size),
										MSG_WAITALL | MSG_NOSIGNAL,
										(struct sockaddr*)&client->info,
										&client->lenInfo
									);
				if(result != ntohs(message->size)){ return	-1; }

				return	0;
			}
			int32_t	writeMessage
							(
								int32_t	sock,
								Message*	restrict	message,
								Client*	restrict	client
							)
			{
				static	int32_t	result	=	0;
	
				errno	=	EINVAL;
	
				if(!sock){ return	-1; }
				if(!message){ return	-1; }
				if(!client){ return	-1; }
	
				errno	=	0;

				client->lenInfo	=	sizeof(struct sockaddr_in);
	
				result	=	sendto
									(
										sock,
										&message->size,
										sizeof(uint16_t),
										MSG_WAITALL | MSG_NOSIGNAL,
										(struct sockaddr*)&client->info,
										client->lenInfo
									);
				if(result != sizeof(uint16_t)){ return	-1; }
				result	=	sendto
									(
										sock,
										message->content,
										ntohs(message->size),
										MSG_WAITALL | MSG_NOSIGNAL,
										(struct sockaddr*)&client->info,
										client->lenInfo
									);
				if(result != ntohs(message->size)){ return	-1; }

				return	0;
			}
//----------------------------------------------------------------
