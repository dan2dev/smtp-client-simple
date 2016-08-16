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



int main() {
	puts("Simples Client de SMTP\n");

	// variaveis gerais
	char *mensagem;










	printf("\n----------------------------------------------------\n");
	// carregar mensagem na memória ------------------------------
	long fileSize = 0;
	FILE *file = fopen ( "smtp-email.txt" , "rb" );
	// descobrir tamanho do arquivo
	fseek( file , 0L , SEEK_END); // final do arquivo
	fileSize = ftell( file ); // ftell - pegar tramanho do arquivo em char
	rewind( file ); // volta pro incio

	// alocar memória para a mensagem
	mensagem = (char *) malloc (fileSize + 1); // tamanho do char da mensagem = filzSize
	fread( mensagem , fileSize, 1 , file); // ler o arquivo e armazenar na "mensagem"
	fclose(file);
	// ------------------------
	printf("%s", mensagem);
	printf("\n----------------------------------------------------\n");













	int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];

    // Criar socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    puts("Socket criado \n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 25 );

	// Conectar ao servidor remoto
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("Erro ao tentar se contectar.");
        return 1;
    }
    puts("Conectado\n");





	//Send some data
	char *helo = "HELO gmail.com 25\n";
	char *mailFrom = "MAIL FROM: danilo@ubuntu\n";
	char *mailTo = "RCPT TO: ccastro.danilo@gmail.com\n";
	char *data = "DATA\nSubject: Teste de email aqui.\n\nMensagem teste aqui. Lorem ipsum dolor sit amet.\n.\n";
	if( send(sock , helo , strlen(helo) , 0) < 0) {	puts("Send failed"); return 1; }
	//Receive a reply from the server
	if( recv(sock , server_reply , 2000 , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , mailFrom , strlen(mailFrom) , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , 2000 , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , mailTo , strlen(mailTo) , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , 2000 , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , data , strlen(data) , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , 2000 , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);

	if( send(sock , "QUIT\n" , 5 , 0) < 0) {	puts("Send failed"); return 1; }
	if( recv(sock , server_reply , 2000 , 0) < 0) {	puts("recv failed"); }
	puts(server_reply);


    close(sock);
	puts("ok");


	// transmitir -----------------------------------------
	int linha =0;
	int linhaSize = 0;
	int caracter=0;
	while(mensagem[caracter]!='\0') {
		if(mensagem[caracter]!='\n') {
			linhaSize++;
			printf("%c",mensagem[caracter]);
		} else {
			linha++;
			linhaSize = 0;
			printf("\n");
		}
		caracter++;
	}



	return 0;
}


// notes postfix ------------------------------
/*
System mail name: ubuntu

*/
