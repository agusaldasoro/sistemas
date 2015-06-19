#include "srv.h"

/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */

void servidor(int mi_cliente){
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    int puedo_escribir;
    int mi_rank = mi_cliente - 1;
    int numero_pedido = 0;
    int pedido_mas_alto = 0;
    int recibido[1];
    int i;
    int me_pidieron[cant_ranks];
    int a_quien_pedi[cant_ranks];
    int siguen_activos[cant_ranks];
    int server_vivos = cant_ranks/2;
    for (i = 0; i < cant_ranks; i+=2){
        me_pidieron[i] = FALSE;
        a_quien_pedi[i] = FALSE;
        siguen_activos[i] = TRUE;
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
            hay_pedido_local = TRUE;
            numero_pedido = pedido_mas_alto+1;
            puedo_escribir = 1;
            if(server_vivos == 1)
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            else{
                recibido[0] = numero_pedido;
                for (i = 0; i < cant_ranks; i+=2){
                    if(i != mi_rank && siguen_activos[i] == TRUE){
                        MPI_Send(recibido, 1, MPI_INT, i, TAG_QUIERO_ESCRIBIR, COMM_WORLD);
                        a_quien_pedi[i] = TRUE;
                    }
                }
            }
        }

        else if (tag == TAG_PODES_ESCRIBIR){
            if(a_quien_pedi[origen] == TRUE){
                puedo_escribir++;
                a_quien_pedi[origen] = FALSE;
                if(puedo_escribir >= server_vivos)
                    MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
        }

        else if (tag == TAG_QUIERO_ESCRIBIR){
            pedido_mas_alto = pedido_mas_alto > recibido[0] ? pedido_mas_alto : recibido[0];
            if(hay_pedido_local == TRUE && (recibido[0] > numero_pedido || (recibido[0] == numero_pedido && origen > mi_rank)))
                me_pidieron[origen] = TRUE;
            else{
                MPI_Send(NULL, 0, MPI_INT, origen, TAG_PODES_ESCRIBIR, COMM_WORLD);
            }
        }

        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);

            for (i = 0; i < cant_ranks; i+=2){
                if(me_pidieron[i] == TRUE){
                    MPI_Send(NULL, 0, MPI_INT, i, TAG_PODES_ESCRIBIR, COMM_WORLD);
                }
                me_pidieron[i] = FALSE;
            }
            hay_pedido_local = FALSE;

        }
        
        else if (tag == TAG_ADIOS){
            debug("me avisan que terminan");
            siguen_activos[origen] = FALSE;
            siguen_activos[origen+1] = FALSE;
            server_vivos--;
            if(a_quien_pedi[origen] == TRUE){
                if(puedo_escribir >= server_vivos)
                    MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
            else{
                puedo_escribir--;
            }
            a_quien_pedi[origen] = FALSE;
        }

        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;
//            siguen_activos[mi_rank] = FALSE;
//            siguen_activos[mi_cliente] = FALSE;
            for (i = 0; i < cant_ranks; i+=2){
                if(siguen_activos[i] == TRUE)
                    MPI_Send(NULL, 0, MPI_INT, i, TAG_ADIOS, COMM_WORLD);
            }
        }
        
    }
    
}

