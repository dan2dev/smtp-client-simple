#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* close */
#include <string.h>
#define MAX_MSG 1000

void getns( char *buf, int tam ) {
	int i;
	for( i = 0; i < tam-1; ++i ) {
		buf[i] = getchar();
		if( buf[i] == '\n' ) {
			buf[i] = '\0';
			return;
		}
	}
	buf[i] = '\0';
}

int main (int argc, char *argv[]) {
	int sd, rc;
	struct sockaddr_in localAddr, servAddr;
	struct hostent *h;
	char buffer[MAX_MSG], rec_msg[MAX_MSG];
	int recebidos;

	if(argc < 3) {
		printf("uso: %s <servidor> <porta>\n",argv[0]);
		exit(1);
	}

	h = gethostbyname(argv[1]);
	if(h==NULL) {
		printf("%s: host desconhecido '%s'\n",argv[0],argv[1]);
		exit(1);
	}

	int porta = atoi(argv[2]);

	servAddr.sin_family = h->h_addrtype;
	memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	servAddr.sin_port = htons(porta);

	/* cria o socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd<0) {
		perror("problemas ao abrir socket ");
		exit(1);
	}

	/* amarra (bind) qualquer porta local */
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(0);

	rc = bind(sd, (struct sockaddr *) &localAddr, sizeof(localAddr));
	if(rc<0) {
		printf("%s: problemas ao amarrar (bind) a porta TCP %u\n", argv[0], porta);
		perror("erro ");
		exit(1);
	}

	/* conecta ao servidor */
	rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
	if(rc<0) {
		perror("problemas ao conectar ");
		exit(1);
	}
	
	/* recebe a mensagem inicial */
	recebidos = recv( sd, rec_msg, MAX_MSG-1, 0 );
    if(recebidos<0) {
		perror("problemas ao receber os dados ");
		close(sd);
		exit(1);
    }
	rec_msg[recebidos] = '\0';
	printf( "Mensagem inicial: %s\n", rec_msg );

	// Pede ao usuário a frase a enviar
	printf( "Digite a frase a enviar: " );
	getns( buffer, MAX_MSG );

	// Envia a frase
	rc = send(sd, buffer, strlen(buffer) + 1, 0);
    if(rc<0) {
		perror("problemas ao enviar dados ");
		close(sd);
		exit(1);
    }
	
	// Recebe a resposta
	recebidos = recv( sd, rec_msg, MAX_MSG-1, 0 );
    if(recebidos < 0) {
		perror("problemas ao receber os dados ");
		close(sd);
		exit(1);
    }
	rec_msg[recebidos] = '\0';
	printf( "Resposta: %s\n", rec_msg );

	// Fecha o socket
	close(sd);

	return 0;
}
