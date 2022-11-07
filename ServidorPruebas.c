#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	char buff[512];
	//INICIALITZACIONS
	//Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error creant el socket");
	} 
	// Fem el bind al port
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// escucharemos en el port 9050
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) < 0)
		printf("Error al bind");
	//La cola de peticiones pendientes no podr? ser superior a 4
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	int i;
	// Atenderemos solo 5 peticiones
	for (i = 0; i < 7; i++) {
		printf("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexi?n\n");
		//sock_conn es el socket que usaremos para este cliente
		
		// Ahora recibimos su nombre, que dejamos en buff
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret] = '\0';
		
		//Escribimos el nombre en la consola
		
		printf("Se ha conectado: %s\n", peticion);
		
		
		char *p = strtok(peticion, "/");
		int codigo = atoi(p);
		p = strtok(NULL, "/");
		char buff[20];
		strcpy(buff, p);
		p = strtok(NULL, "/");
		char buff2[20];
		strcpy(buff2, p);
		printf("Codigo: %d, Parametro: %s\n", codigo, buff, buff2);
		
		
		if (codigo == 1) 
		{
			i = LogIn(buff, buff2);
			if(i == 0) strcpy(respuesta,"Te has logeado correctamente");
			else strcpy(respuesta,"No has podido iniciar sesión");
				write(sock_conn, respuesta,strlen(respuesta));
		}
		else if (codigo == 2)
		{
			
		}
		
		// Se acabo el servicio para este cliente
		close(sock_conn);
	}
}
int LogIn(char username[20], char password[20])
{
	MYSQL* conn;
	int err;
	MYSQL_RES* resultado;
	MYSQL_ROW row;
	char consulta[80];
	char res[80];
	//parte de mysql
	char user[20], pass[20]; //variables para comparar nombre y contraseña
	
	conn = mysql_init(NULL);
	
	if (conn == NULL)
	{
		printf("Error al crear la conexion: %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
	
	conn = mysql_real_connect(conn, "localhost", "root", "mysql", "bd_final", 0, NULL, 0);
	
	if (conn == NULL)
	{
		printf("Error al inicializar la conexion: %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
	
	strcpy(consulta, "SELECT JUGADOR.USERNAME,JUGADOR.PASSWORD FROM JUGADOR WHERE JUGADOR.USERNAME = '");
	strcat(consulta, username);
	strcat(consulta, "'");
	printf(consulta);
	err = mysql_query(conn, consulta);
	
	if (err != 0)
	{
		printf("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
	resultado = mysql_store_result(conn);
	row = mysql_fetch_row(resultado);
	
	if (row == NULL)
		printf("No se han obtenido datos en la consulta\n");
	
	else
	{
		strcpy(user, row[0]);
		strcpy(pass, row[1]);
		
		printf("nombre recibido: %s, contra recibida: %s\n", user, pass);
		row = mysql_fetch_row(resultado);
	}
	
	if ((strcmp(user, username) == 0) && (strcmp(pass, password) == 0))
	{
		return 0;
	}
	
	else
										  return 1;
}

