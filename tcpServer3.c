#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <netdb.h>
#define TRUE !0

#define PORTA 1500
#define MAX_MSG 1000
#define ERRO  -1
#define EMPTY 1

int lelinha( int sock, char *msg, int tam_msg, int echo ){
    int i, j, numbytes;

    i = 0;
    numbytes = recv( sock, msg, tam_msg-1, 0);
    if( numbytes < 0 ) {
        perror( "Nao pode receber");
        return ERRO;
    }
    msg[numbytes] = '\0';
    if( echo ) {
        if( send( sock, msg, strlen(msg), 0) < 0 ) {
            perror( "Nao pode enviar");
            return ERRO;
        }
    }
    if( numbytes == 0 ){
        return 0;
    } 
    for( i = 0; i < numbytes; i++) {
        if( msg[i] == '\n' || msg[i] == '\r' ) {
            msg[i] = '\0';
            //printf( "Linha: %s\n", msg );
            return i;
        }
    }    

    return i;
}


int main(int argc, char *argv[])
{
    int i, j, conexao, tamanho, lidos;
    int porta = PORTA;
    struct sockaddr_in local, remoto;
    struct hostent *he;
    struct servent *servico;
    char msg_out[MAX_MSG];
    char msg_linha[MAX_MSG];

    /* trabalhando com os parametros de entrada */
    if( argc == 1 ) {
        printf( "Uso: %s <porta>", argv[0] );
        exit(0);
    }
    
    porta = atoi(argv[1]);

    int sc, sc_accept;

    /* Criando o soquete */
    sc = socket( AF_INET, SOCK_STREAM, 0 );
    if( sc < 0 ) {
        perror( "Nao foi possivel criar socket" );
        exit(1);
    }
    
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(porta);
    
    for (i=0;i<8;i++) {
        local.sin_zero[i]=0;
    }
    /*bzero( &(alvo.sin_zero), 8 );*/

    conexao = bind( sc, (struct sockaddr*)&local, sizeof(local) );
    if(conexao < 0) {
        fprintf( stderr, "Nao foi possivel usar a porta %d\n", porta );
        close( sc );
        exit(1);
    }

    listen( sc, 5 );

    while(1) {
	    tamanho = sizeof( struct sockaddr_in );
	    printf( "Servidor Teste: aceitando chamadas na porta %s\n", argv[1] );
	    sc_accept = accept( sc, (struct sockaddr *)&remoto, &tamanho );
	    if(sc_accept < 0) {
	        perror( "Conexao recusada " );
	        continue;
	    }
	    printf("Conexao aceita de %s na porta TCP %d\n", inet_ntoa(remoto.sin_addr),
			ntohs(remoto.sin_port) );
	    strcpy( msg_out, "Servidor de teste\n" );
	    int numbytes;
	    i = 0;
	    if( send( sc_accept, msg_out, strlen(msg_out), 0) == -1 ) {
	        perror( "Nao pode enviar" );
	        close( sc_accept );
	        close( sc );
	        exit(1);
	    }

	    while( (lidos = lelinha( sc_accept, msg_linha, MAX_MSG, TRUE )) != -1 ) {
	        printf( "Linha: %s\n", msg_linha );
	        if( !strcmp( msg_linha, "exit" ) )
	            break;
	    }

	    close( sc_accept );

	    if( lidos < 0 ) {
	      close(sc);
	      exit(ERRO);
	    }
    } /* while (1) do accept() */


    close(sc);

    return 0;
}


