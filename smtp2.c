#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <err.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



// -r = remetente
// -d = destinatario
// -f = Filename arquivo

#define MAX_MSG 2000

int main(int argc , char *argv[]) {
	puts("Simples Client de SMTP\n");

	// variaveis de server
	struct hostent *h;
	struct sockaddr_in servAddr;

	// variaveis gerais
	char *argHost = "127.0.0.1";
	int argHostPort = 25;
	char *mensagem;
	char *argMailFrom = "danilo@ubuntu";
	char *argMailTo = "danilo@ubuntu";
	char *argFileName = "smtp-email.txt";



	// Argumentos
	printf("\n----------------------------------------------------\n");
	if (!(argc % 2)) {
		// se for impar mandar erro
		printf("Erro. Argumentos inválidos!");
		return 1;
	} else {
		// printf("Argumtno: %d", i);
		for (int i = 0; i < argc; i++) {
			// printf("Argumentos: %s/n", argv[i]);
			/// comparar pra ver se o tipo de argumento
			// arquivo ------------------------------------
			if (!strcmp(argv[i],"-f")) {
				argFileName = (char *) malloc(strlen(argv[i+1])+1);
				strcpy(argFileName,argv[i+1]);
			}
			if (!strcmp(argv[i],"-r")) {
				argMailFrom = (char *) malloc(strlen(argv[i+1])+1);
				strcpy(argMailFrom,argv[i+1]);
			}
			if (!strcmp(argv[i],"-d")) {
				argMailTo = (char *) malloc(strlen(argv[i+1])+1);
				strcpy(argMailTo,argv[i+1]);
			}
			if (!strcmp(argv[i],"-p")) {
				char *hostPortTemp = (char *) malloc(strlen(argv[i+1])+1);
				hostPortTemp = (char *) malloc(strlen(argv[i+1])+1);
				strcpy(hostPortTemp,argv[i+1]);
				argHostPort =  atoi(hostPortTemp);
				hostPortTemp = NULL;
			}
			if (!strcmp(argv[i],"-s")) {
				// printf("host: %s\n", argv[i+1]);
				h = gethostbyname(argv[i+1]);
				if(h==NULL) {
					printf("%s: host desconhecido \n",argv[i+1]);
					return 1;
				}
				servAddr.sin_family = h->h_addrtype;
				memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
				argHost = (char *) malloc(strlen(inet_ntoa(servAddr.sin_addr)));
				sprintf(argHost, "%s", inet_ntoa(servAddr.sin_addr));
			}
		}
		printf("remetente: %s\n", argMailFrom);
		printf("destinatario: %s\n", argMailTo);
		printf("arquivo: %s\n", argFileName);
		printf("host: %s:%d\n", argHost, argHostPort);
	}















	// printf("\n----------------------------------------------------\n");
	// carregar mensagem na memória ------------------------------
	long fileSize = 0;
	// FILE *file = fopen ( "smtp-email.txt" , "rb" );
	FILE *file = fopen ( argFileName , "rb" );
	// descobrir tamanho do arquivo
	fseek( file , 0L , SEEK_END); // final do arquivo
	fileSize = ftell( file ); // ftell - pegar tramanho do arquivo em char
	rewind( file ); // volta pro incio

	// alocar memória para a mensagem
	mensagem = (char *) malloc (fileSize + 1); // tamanho do char da mensagem = filzSize
	fread( mensagem , fileSize, 1 , file); // ler o arquivo e armazenar na "mensagem"
	fclose(file);
	// ------------------------
	//printf("%s", mensagem);
	printf("\nArquivo de mensagem carregado \n");













	int sock;
    struct sockaddr_in server;
    char  server_reply[MAX_MSG];

    // Criar socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    puts("Socket criado \n");

    server.sin_addr.s_addr = inet_addr(argHost);
    server.sin_family = AF_INET;
    server.sin_port = htons( argHostPort );

	// Conectar ao servidor remoto
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("Erro ao tentar se contectar.");
        return 1;
    }
    puts("Conectado\n");





	//Send some data
	char *helo = "HELO localhost 25\n";
	// mailFrom ---------------------------------------------------------------------------
	char *mailFromCommandStr = "MAIL FROM: ";
	char *mailFrom =  (char *) malloc(strlen(mailFromCommandStr) + strlen(argMailFrom)+1);
	sprintf(mailFrom, "%s%s\n", mailFromCommandStr, argMailFrom);

	// mailTo -----------------------
	// char *mailTo = "RCPT TO: danilo@ubuntu\n";
	char *mailToCommand = "RCPT TO: ";
	char *mailTo =  (char *) malloc(strlen(mailToCommand) + strlen(argMailTo)+1);
	sprintf(mailTo, "%s%s\n", mailToCommand, argMailTo);
	// mensagem ------------------------------------------------------------------------------
	// char *data = "DATA\nSubject: Teste de email aqui.\n\nMensagem teste aqui. Lorem ipsum dolor sit amet.\n.\n";
	char *dataCommand = "DATA\nSubject: email teste.\n\n";
	char *data = (char *) malloc(strlen(dataCommand) + strlen(mensagem)+1);
	sprintf(data, "%s%s\n.\n", dataCommand, mensagem);

	// send -----------------------------------------
	if( send(sock , helo , strlen(helo) , 0) < 0) {	puts("Send failed"); return 1; }
	//Receive a reply from the server
	if( recv(sock , server_reply , MAX_MSG , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , mailFrom , strlen(mailFrom) , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , MAX_MSG , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , mailTo , strlen(mailTo) , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , MAX_MSG , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , data , strlen(data) , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , MAX_MSG , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , "QUIT\n" , 5 , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , MAX_MSG , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);


    close(sock);
	puts("ok");


	// transmitir -----------------------------------------
	// int linha =0;
	// int linhaSize = 0;
	// int caracter=0;
	// while(mensagem[caracter]!='\0') {
	// 	if(mensagem[caracter]!='\n') {
	// 		linhaSize++;
	// 		printf("%c",mensagem[caracter]);
	// 	} else {
	// 		linha++;
	// 		linhaSize = 0;
	// 		printf("\n");
	// 	}
	// 	caracter++;
	// }



	return 0;
}



// notes postfix ------------------------------
/*
System mail name: ubuntu
echo "corpo do email" | mail -s "email teste" -a "From: danilo@ubundu" ccastro.danilo@gmail.com


written by: Danilo Celestinod e Castro
*/
