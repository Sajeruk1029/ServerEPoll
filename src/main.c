/**
	*	@file
	*	@brief ПО Server. Точка входа.
	*	@author Кузнецов Егор Ильич goga.petrovich12@gmail.com
	*	@version 1.0.0
	*	@date 29.11.2025
*/

/**
	*@mainpage
	*
	*	Server
	*
	*	Server -- Это программное обеспечение, которое предоставляет серверы TCP и UDP с мультиплексированием через класс системных вызовов EPoll.
	*
	*
	*	ПО Server принимает струтуры, которые представляют собой сообщения, которые состоят из текста размером не более 1024 байт и дилны сообщения 2 байта. Прием и передача происходят в сетевом порядке байт(Big Endian). Сервер осуществляет работу с 2 видами сообщений: обычные, текстовые и команды.
	*
	*
	*	Команды характеризуются наличием символа / в начале строки.
	*
	*	Поддерживаемые команды сервером:
	*	/time	-	Возвращает строку времени в формате yyyy-MM-dd hh:mm:ss.
	*	/stats	-	Возвращает статистику о подключенных за все время работы пользователях и подключенных в данный момент. В формате Connections: x Current connections: x
	*	/shutdown	-	Выключение сервера.
	*
	*
	*	Запуск ПО Server производится следующим образом:
	*	./Server -p 5555 -P 5556
	*
	*	Аргументы -p(Порт TCP) и -P(Порт UDP) являются обязательными.
	*	Общий набор аргументов:
	*	-p / --port-tcp -- Порт TCP
	*	-P / --port-udp -- Порт UDP
	*	-t / --timeout -- Таймаут ожидания сообщений в мс. По умолчанию 1 мс.
	*	-c / --connections -- Максимальное количество клиентов. По умолчанию 2.
	*	-o / --out-file -- Выходной лог файл.
	*
	*	ПО Server работает в режиме демона и после запуска уходит в фоновый режим, отключаясь от управляющего терминала. Стандартные потоки ввода-вывода при это направляются в /dev/null, а рабочий каталог ПО меняется на /.
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
//		C Заголовки
//----------------------------------------------------------------
#include	<stdio.h>
#include	<stdint.h>
#include	<errno.h>
#include	<string.h>
#include	<stdlib.h>
#include	<time.h>
#include	<malloc.h>
#include	<ctype.h>
//----------------------------------------------------------------
//		C Linux Заголовки
//----------------------------------------------------------------
#include	<sys/fcntl.h>
#include	<sys/unistd.h>
#include	<sys/socket.h>
#include	<sys/epoll.h>

#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<getopt.h>
//----------------------------------------------------------------
//		C собственные заголовки
//----------------------------------------------------------------
#include	<Message.h>
#include	<Client.h>
#include	<log.h>
//----------------------------------------------------------------
//	Макросы
//----------------------------------------------------------------
/**
*	@brief	Максимальное количество клиентов по умолчанию. 
*	@details	Максимальное количество клиентое, которое будет установлен в том случае, если ничего не было указано.
*/
	#define	MAXCONS	2
/**
*	@brief	Таймаут по умолчанию. 
*	@details	Таймаут в милисекундах, который будет установлен в том случае, если ничего не было указано.
*/
	#define	TIMEOUT	1
//----------------------------------------------------------------
//	Функции
//----------------------------------------------------------------
/**
*	@brief	Вывод справки.
*	@details	Вывод справки.
*
*	@param[in]	app	Имя программы.
*
*	@return	0, в случае успеха. -1, в случае ошибки. Устанавливает глобальное значение errno в соответствии с ошибкой.
*/
		int32_t	help(char*	app)
		{
			if(!app)
			{
				errno	=	EINVAL;

				return	-1;
			}

      fprintf
			(
				stdout,
				"%s\n\t\t%s\t%s\n\t\t\t%s\n",
				"Usage",
				app,
				"[options]",
				"-p <value> / --port-tcp <value>\n\t\t\t"
				"-P <value> / --port-udp <value>\n\t\t\t"
				"-t <value> / --timeout-ms <value> | default 1\n\t\t\t"
				"-c <value> / --connections <value> | default 2\n\t\t\t"
				"-o <value> / --out-file <value>\n\t\t\t"
				"-h / --help"
			);

			return	0;
		}

		/**
		*	@brief	Проверка строки на число.
		*	@details	Проверяет, является ли строка, числом.
		*
		*	@param[in]	line	Строка.
		*
		*	@return	1, если является. 0, если не является.
		*/
		int32_t	isNumber(char*	restrict	line)
		{
			while(*line){ if(!isdigit(*line++)){ return	0; } }

			return	1;
		}

//----------------------------------------------------------------
//	Точка входа
//----------------------------------------------------------------
		int	main(int	argc, char**	argv)
		{
			const	struct option	longParams[]	=
			{
				{ "port-tcp", 1, 0x00, 'p' },
				{ "port-udp", 1, 0x00, 'P' },
				{ "timeout-ms", 1, 0x00, 't' },
				{ "connections", 1, 0x00, 'c' },
				{ "out-file", 1, 0x00, 'o' },
				{ "help", 0, 0x00, 'h' },
				{ 0x00, 0, 0x00, 0 }
			};

			Message	message	=	{ { 0 }, 0 };

			Client	client	=	{ { 0 }, sizeof(struct sockaddr_in) };

			struct epoll_event	event	=	{ 0 };

			struct sockaddr_in	serverTCP	=	{ 0 };
			struct sockaddr_in	serverUDP	=	{ 0 };

			struct epoll_event*	events	=	0x00;

			const	char*	shortParams	=	"p:P:t:c:o:h";

			FILE*	fileOut	=	0x00;

			char*	out	=	0x00;

			int32_t	result	=	0;
			int32_t	sockTCP	=	0;
			int32_t	sockUDP	=	0;
			int32_t	handlerRecv	=	0;
			int32_t	eventsSize	=	0;

			uint32_t	timeout	=	0;
			uint32_t	maxConnections	=	0;

			uint16_t	portTCP	=	0;
			uint16_t	portUDP	=	0;
			uint16_t	connections	=	0;
			uint16_t	currentConnections	=	0;

			char	symbol	=	0;

			while((symbol	=	getopt_long(argc, argv, shortParams, longParams, 0x00)) != -1)
			{
				switch(symbol)
				{
					case	'p':
					{
						if(!isNumber(optarg))
						{
							help(*(argv + 0));

							fprintf(stderr, "%s\n", "Port TCP must be a number!");

							return	EXIT_FAILURE;
						}

						portTCP	=	atoi(optarg);

						break;
					}
					case	'P':
					{
						if(!isNumber(optarg))
						{
							help(*(argv + 0));

							fprintf(stderr, "%s\n", "Port UDP must be a number!");

							return	EXIT_FAILURE;
						}

						portUDP	=	atoi(optarg);

						break;
					}
					case	't':
					{
						if(!isNumber(optarg))
						{
							help(*(argv + 0));

							fprintf(stderr, "%s\n", "Timeout must be a number!");

							return	EXIT_FAILURE;
						}

						timeout	=	atoi(optarg);

						break;
					}
					case	'c':
					{
						if(!isNumber(optarg))
						{
							help(*(argv + 0));

							fprintf(stderr, "%s\n", "Max connections must be a number!");

							return	EXIT_FAILURE;
						}

						maxConnections	=	atoi(optarg);

						break;
					}
					case	'o':
					{
						out	=	optarg;

						break;
					}
					case	'h':
					{
						help(*(argv + 0));

						return	EXIT_SUCCESS;
					}
					default:
					{
						help(*(argv + 0));

						return	EXIT_FAILURE;
					}
				}
			}

			if(!portTCP)
			{
				fprintf(stderr, "%s\n", "Port TCP must be defined!");

				help(*(argv + 0));

				return	-1;
			}
			if(!portUDP)
			{
				fprintf(stderr, "%s\n", "Port UDP must be defined!");

				help(*(argv + 0));

				return	-1;
			}

			if(!timeout){ timeout	=	TIMEOUT; }
			if(!maxConnections){ maxConnections	=	MAXCONS; }

			if(out)
			{
				fileOut	=	fopen(out, "w");
				if(!fileOut)
				{
					fprintf(stderr, "%s\n", strerror(errno));

					return	EXIT_FAILURE;
				}
			}

			result	=	daemon(0, 0);
			if(result < 0)
			{
				fprintf(stderr, "%s\n", strerror(errno));

				return	EXIT_FAILURE;
			}

			events	=	(struct epoll_event*)calloc(maxConnections, sizeof(struct epoll_event));
			if(!events)
			{
				fprintf(stderr, "%s\n", strerror(errno));

				if(fileOut)
				{
					fclose(fileOut);

					fileOut	=	0x00;
				}

				exit(EXIT_FAILURE);
			}

			event.events	=	EPOLLIN | EPOLLET;

			handlerRecv	=	epoll_create(maxConnections);
			if(handlerRecv < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}

			serverTCP.sin_family	=	AF_INET;
			serverTCP.sin_port	=	htons(portTCP);
			serverTCP.sin_addr.s_addr	=	0;

			serverUDP.sin_family	=	AF_INET;
			serverUDP.sin_port	=	htons(portUDP);
			serverUDP.sin_addr.s_addr	=	0;

			sockTCP	=	socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
			if(sockTCP < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}
				close(handlerRecv);

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}
			sockUDP	=	socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if(sockUDP < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}

				close(sockTCP);

				close(handlerRecv);

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}

			result	=	bind(sockTCP, (struct sockaddr*)&serverTCP, sizeof(struct sockaddr_in));
			if(result < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}

				close(sockUDP);
				close(sockTCP);

				close(handlerRecv);

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}
			result	=	bind(sockUDP, (struct sockaddr*)&serverUDP, sizeof(struct sockaddr_in));
			if(result < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}

				close(sockUDP);
				close(sockTCP);

				close(handlerRecv);

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}

			event.data.fd	=	sockUDP;
			result	=	epoll_ctl(handlerRecv, EPOLL_CTL_ADD, sockUDP, &event);
			if(result < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}

				close(sockUDP);
				close(sockTCP);

				close(handlerRecv);

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}

			result	=	fcntl(sockTCP, F_SETFL, O_NONBLOCK);
			if(result < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}

				close(sockUDP);
				close(sockTCP);

				close(handlerRecv);

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}

			result	=	listen(sockTCP, maxConnections);
			if(result < 0)
			{
				if(fileOut)
				{
					printErrnoError(fileOut);

					fclose(fileOut);

					fileOut	=	0x00;
				}

				close(sockUDP);
				close(sockTCP);

				close(handlerRecv);

				free(events);

				events	=	0x00;

				exit(EXIT_FAILURE);
			}

			while(1)
			{
				result	=	accept(sockTCP, 0x00, 0x00);
				if(result < 0)
				{
					if(errno != EAGAIN)
					{
						if(fileOut)
						{
							printErrnoError(fileOut);

							fclose(fileOut);

							fileOut	=	0x00;
						}

						close(sockUDP);
						close(sockTCP);

						close(handlerRecv);

						free(events);

						events	=	0x00;

						exit(EXIT_FAILURE);
					}
				}
				else
				{
					event.data.fd	=	result;
					result	=	epoll_ctl(handlerRecv, EPOLL_CTL_ADD, result, &event);
					if(result < 0)
					{
						if(fileOut)
						{
							printErrnoError(fileOut);

							fclose(fileOut);

							fileOut	=	0x00;
						}

						close(sockUDP);
						close(sockTCP);

						close(handlerRecv);

						free(events);

						events	=	0x00;

						exit(EXIT_FAILURE);
					}

					++connections;
					++currentConnections;
				}

				eventsSize	=	epoll_wait(handlerRecv, events, maxConnections, TIMEOUT);
				if(eventsSize < 0)
				{
					if(fileOut)
					{
						printErrnoError(fileOut);

						fclose(fileOut);

						fileOut	=	0x00;
					}

					close(sockUDP);
					close(sockTCP);

					close(handlerRecv);

					free(events);

					events	=	0x00;

					exit(EXIT_FAILURE);
				}
				else if(!eventsSize){ continue; }
				else
				{
					for
					(
						struct epoll_event*	currentEvent = events;
						currentEvent < events + eventsSize;
						++currentEvent
					)
					{
						result	=	readMessage(currentEvent->data.fd, &message, &client);
						if(result < 0)
						{
							if(!errno)
							{
								--currentConnections;

								epoll_ctl(handlerRecv, EPOLL_CTL_DEL, currentEvent->data.fd, 0x00);

								continue;
							}

							if(fileOut)
							{
								printErrnoError(fileOut);

								fclose(fileOut);

								fileOut	=	0x00;
							}

							close(sockUDP);
							close(sockTCP);

							close(handlerRecv);

							free(events);

							events	=	0x00;

							exit(EXIT_FAILURE);
						}

						if(!strchr(message.content, '/'))
						{
							result	=	writeMessage(currentEvent->data.fd, &message, &client);
							if(result < 0)
							{
								if(!errno)
								{
									--currentConnections;

									epoll_ctl(handlerRecv, EPOLL_CTL_DEL, currentEvent->data.fd, 0x00);

									continue;
								}

								if(fileOut)
								{
									printErrnoError(fileOut);

									fclose(fileOut);

									fileOut	=	0x00;
								}

								close(sockUDP);
								close(sockTCP);

								close(handlerRecv);

								free(events);

								events	=	0x00;

								return	EXIT_FAILURE;
							}
						}
						else
						{
							if(strstr(message.content, "time"))
							{
								messageClear(&message);

								messageTime(&message, time(0x00));

								messageHostToNet(&message);

								result	=	writeMessage(currentEvent->data.fd, &message, &client);
								if(result < 0)
								{
									if(!errno)
									{
										--currentConnections;

										epoll_ctl(handlerRecv, EPOLL_CTL_DEL, currentEvent->data.fd, 0x00);

										continue;
									}

									if(fileOut)
									{
										printErrnoError(fileOut);

										fclose(fileOut);

										fileOut	=	0x00;
									}

									close(sockUDP);
									close(sockTCP);

									close(handlerRecv);

									free(events);

									events	=	0x00;

									exit(EXIT_FAILURE);
								}
							}
							else if(strstr(message.content, "stats"))
							{
								messageClear(&message);

								messageStats(&message, connections, currentConnections);

								messageHostToNet(&message);

								result	=	writeMessage(currentEvent->data.fd, &message, &client);
								if(result < 0)
								{
									if(!errno)
									{
										--currentConnections;

										epoll_ctl(handlerRecv, EPOLL_CTL_DEL, currentEvent->data.fd, 0x00);

										continue;
									}

									if(fileOut)
									{
										printErrnoError(fileOut);

										fclose(fileOut);

										fileOut	=	0x00;
									}

									close(sockUDP);
									close(sockTCP);

									close(handlerRecv);

									free(events);

									events	=	0x00;

									exit(EXIT_FAILURE);
								}
							}
							else if(strstr(message.content, "shutdown"))
							{
								close(sockUDP);
								close(sockTCP);

								close(handlerRecv);

								free(events);

								events	=	0x00;

								if(fileOut)
								{
									fclose(fileOut);

									fileOut	=	0x00;
								}

								exit(EXIT_SUCCESS);
							}
						}

						messageClear(&message);
					}
				}
			}

			close(sockUDP);
			close(sockTCP);

			close(handlerRecv);

			free(events);

			events	=	0x00;

			fclose(fileOut);

			fileOut	=	0x00;

			return	EXIT_SUCCESS;
		}
