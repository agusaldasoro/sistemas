#include "srv.h"

void servidor(int mi_cliente){
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;//si mi cliente pidio acceso exclusivo
    int listo_para_salir = FALSE;//si mi cliente me indica que se retira
    int puedo_escribir;//cuantos otros servidores me indicaron que puedo obtener el acceso exclusivo
    int mi_rank = mi_cliente - 1;//los servidores tienen rank 2i, sus clientes 2i+1
    int numero_pedido = 0;//mi numero de pedido de acceso exclusivo
    int pedido_mas_alto = 0;//el numero de pedido mas alto que alguna vez me pidio, o el mio
    int recibido[1];//arreglo de un espacio para enviar el numero de pedido
    int i;//contador para los for
    int me_pidieron[cant_ranks];//registro de a quien le debo permisos porque me pidieron, pero yo era mas importante
    int a_quien_pedi[cant_ranks];//registro de quienes me dijeron que podia tener el acceso exclusivo
    int siguen_activos[cant_ranks];//registro de que servidores y clientes siguen activos
    int server_vivos = cant_ranks/2;//cuantos servidores siguen activos
    for (i = 0; i < cant_ranks; i+=2){//por cada servidor
        me_pidieron[i] = FALSE;//nadie me pidio nada
        a_quien_pedi[i] = FALSE;//no le pedi a nadie
        siguen_activos[i] = TRUE;//todos arrancan activos
    }

    while( ! listo_para_salir ) {

        MPI_Recv(recibido, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
//        MPI_Recv(NULL, 0, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
        if (tag == TAG_PEDIDO) {
            assert(origen == mi_cliente);
            debug("Mi cliente solicita acceso exclusivo");
            assert(hay_pedido_local == FALSE);
            hay_pedido_local = TRUE;//voy a pedir acceso exclusivo
            numero_pedido = pedido_mas_alto+1;//mi pedido sera el siguiente al mas alto pedido hasta el momento
            puedo_escribir = 1;//yo me doy permiso, me faltan los demas
            if(server_vivos == 1)//si soy el unico que quedo, ya tengo permisos
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            else{
                recibido[0] = numero_pedido;//voy a enviar mi numero de pedido en el arreglo
                for (i = 0; i < cant_ranks; i+=2){//a cada servidor
                    if(i != mi_rank && siguen_activos[i] == TRUE){//si no soy yo y sigue activo, le aviso que quiero acceso exclusivo
                        MPI_Send(recibido, 1, MPI_INT, i, TAG_QUIERO_ESCRIBIR, COMM_WORLD);
                        a_quien_pedi[i] = TRUE;//marco que le pedi acceso al servidor i
                    }
                }
            }
        }

        else if (tag == TAG_PODES_ESCRIBIR){
            if(a_quien_pedi[origen] == TRUE){//si me contesto alguien que me debia respuesta
                puedo_escribir++;//tengo una respuesta mas
                a_quien_pedi[origen] = FALSE;//marco que me contesto
                if(puedo_escribir >= server_vivos)//si ya tengo los permisos necesarios, mi cliente ejecuta
                    MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
        }

        else if (tag == TAG_QUIERO_ESCRIBIR){
            pedido_mas_alto = pedido_mas_alto > recibido[0] ? pedido_mas_alto : recibido[0];//llego un pedido, actualizo el pedido mas alto
            if(hay_pedido_local == TRUE && (recibido[0] > numero_pedido || (recibido[0] == numero_pedido && origen > mi_rank))){
                //si estoy pidiendo yo tambien Y
                //o bien tengo un numero de pedido menor al del otro servidor, o bien tenemos el mismo pero mi ranking es mas bajo
                me_pidieron[origen] = TRUE;//me guardo que le debo respuesta
            }
            else{
                //si no tiene mas prioridad o no estoy pidiendo acceso exclusivo, le aviso que puede acceder
                MPI_Send(NULL, 0, MPI_INT, origen, TAG_PODES_ESCRIBIR, COMM_WORLD);
            }
        }

        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            for (i = 0; i < cant_ranks; i+=2){//para cada servidor
                if(me_pidieron[i] == TRUE){//si me habia pedido acceso, se lo doy, pues mi cliente ya salio de la seccion critica
                    MPI_Send(NULL, 0, MPI_INT, i, TAG_PODES_ESCRIBIR, COMM_WORLD);
                }
                me_pidieron[i] = FALSE;//marco que no le debo nada
            }
            hay_pedido_local = FALSE;//marco que no estoy pidiendo nada mas
        }
        
        else if (tag == TAG_ADIOS){
            siguen_activos[origen] = FALSE;//marco que este server no esta mas activo
            server_vivos--;//hay un servidor menos
            if(a_quien_pedi[origen] == TRUE){//si todavia me debia respuesta
                if(puedo_escribir >= server_vivos)//si era el ultimo necesario, mi cliente accede
                    MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
            else{
                //si este server se bajo, ya no me interesa que me haya dado permiso
                puedo_escribir--;
            }
            a_quien_pedi[origen] = FALSE;//marco que no le pedi nada
        }

        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;//mi cliente se va, yo tambien me ire
            for (i = 0; i < cant_ranks; i+=2){//por cada servidor
                if(siguen_activos[i] == TRUE)//si sigue activo, le aviso que me voy
                    MPI_Send(NULL, 0, MPI_INT, i, TAG_ADIOS, COMM_WORLD);
            }
        }
    }
}

