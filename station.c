#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "station.h"

typedef Train *TTrain;
typedef TrainCar *TTrainCar;
typedef TrainStation *TTrainStation;

/* Creeaza o gara cu un numar fix de peroane.
 *
 * platforms_no: numarul de peroane ale garii
 *
 * return: gara creata
 */
TrainStation* open_train_station(int platforms_no) {
    TTrainStation statie = (TTrainStation) malloc(sizeof (TrainStation));
    statie->platforms_no = platforms_no;
    statie->platforms = (TTrain*) malloc(platforms_no * sizeof (TTrain*));
    for (int i = 0; i < platforms_no; ++i) {
        statie->platforms[i] = (TTrain) malloc(sizeof (Train));
        statie->platforms[i]->locomotive_power=-1;
        statie->platforms[i]->train_cars = NULL;
    }
    return statie;
}


/* Elibereaza memoria alocata pentru gara.
 *
 * station: gara existenta
 */
void close_train_station(TrainStation *station) {
    TTrainStation statie = station;
    for (int i = 0; i < statie->platforms_no; ++i) {
        free(statie->platforms[i]->train_cars);
        free(statie->platforms[i]);
    }
    free(statie->platforms);
    free(statie);
}


/* Afiseaza trenurile stationate in gara.
 *
 * station: gara existenta
 * f: fisierul in care se face afisarea
 */
void show_existing_trains(TrainStation *station, FILE *f) {
    FILE *ptr = f;
    TTrainStation statie = station;
    if (statie != NULL) {
        for (int i = 0; i < statie->platforms_no; ++i) {
            fprintf( ptr , "%d: " , i);
            if ( statie->platforms[i]->locomotive_power != -1) {
                fprintf( ptr , "(%d)" , statie->platforms[i]->locomotive_power);
                TTrainCar car = statie->platforms[i]->train_cars;
                while (car != NULL) {
                    fprintf( ptr , "-|%d|" , car->weight);
                    car = car->next;
                }
            }
            putc('\n' , ptr);
        }
    }
}
/* Adauga o locomotiva pe un peron.
 *
 * station: gara existenta
 * platform: peronul pe care se adauga locomotiva
 * locomotive_power: puterea de tractiune a locomotivei
 */
void arrive_train(TrainStation *station, int platform, int locomotive_power) {
    TTrainStation statie = station;
    if (platform >= 0 && platform < statie->platforms_no) {
        if (statie->platforms[platform]->locomotive_power == -1) {
            statie->platforms[platform]->locomotive_power=locomotive_power;
        }
    }
}


/* Elibereaza un peron.
 *
 * station: gara existenta
 * platform: peronul de pe care pleaca trenul
 */
void leave_train(TrainStation *station, int platform) {
    TTrainStation statie = station;
    TTrainCar elim;
    if ( platform >= 0 && platform < statie->platforms_no ) {
        statie->platforms[platform]->locomotive_power = -1;
        while (statie->platforms[platform]->train_cars != NULL) {
            elim = statie->platforms[platform]->train_cars;
            statie->platforms[platform]->train_cars = statie->platforms[platform]->train_cars->next;
            free(elim);
        }
    }
}


/* Adauga un vagon la capatul unui tren.
 *
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 * weight: greutatea vagonului adaugat
 */
void add_train_car(TrainStation *station, int platform, int weight) {
    TTrainStation statie = station;
    if ( platform >= 0 && platform < statie->platforms_no ) {
        if ( statie->platforms[platform]->locomotive_power != -1 ) {
            TTrainCar list = statie->platforms[platform]->train_cars ,  newCar;
            newCar = (TTrainCar) malloc(sizeof (TrainCar));
            newCar->weight = weight;
            newCar->next = NULL;
            if (list == NULL) {
                list = newCar;
                statie->platforms[platform]->train_cars = list;
            } else if (list != NULL) {
                while (list->next != NULL) {
                    list = list->next;
                }
                list->next = newCar;
            }
        }
    }
}


/* Scoate vagoanele de o anumita greutate dintr-un tren.
 *
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 * weight: greutatea vagonului scos
 */
void remove_train_cars(TrainStation *station, int platform, int weight) {
    TTrainStation statie = station;
    int i = 0;
    if (platform >= 0 && platform < statie->platforms_no) {
        if (statie->platforms[platform]->locomotive_power != -1) {
            TTrainCar a = statie->platforms[platform]->train_cars, elim , elim1 , prev;
            TTrainCar helper , t;
            while ( a != NULL && a->weight==weight) {
                elim = a;
                a=a->next;
                statie->platforms[platform]->train_cars = a;
                free(elim);
            }
            helper = a;
            t = helper;
            while ( helper != NULL ) {
                if (helper->next != NULL && helper->next->weight == weight) {
                    elim1 = helper->next;
                    helper->next = helper->next->next;
                    free(elim1);
                } else {
                    helper = helper->next;
                }
            }
            statie->platforms[platform]->train_cars=t;
        }
    }
}


/* Muta o secventa de vagoane dintr-un tren in altul.
 *
 * station: gara existenta
 * platform_a: peronul pe care se afla trenul de unde se scot vagoanele
 * pos_a: pozitia primului vagon din secventa
 * cars_no: lungimea secventei
 * platform_b: peronul pe care se afla trenul unde se adauga vagoanele
 * pos_b: pozitia unde se adauga secventa de vagoane
 */
void move_train_cars(TrainStation *station, int platform_a, int pos_a,
                     int cars_no, int platform_b, int pos_b) {
    TTrainStation statie = station;
    TTrainCar ptr_a = statie->platforms[platform_a]->train_cars , move_sec , prev_a;
    TTrainCar ptr_b = statie->platforms[platform_b]->train_cars , prev;
    int contor_a = 1 , contor_b = 1 , cont_aux = 0 , a = 1 , b = 1 , c = 1 , lungime_a = 0 , lungime_b = 0;
    int aux_cars_no=1;
    TTrainCar aux , helper , tmp , numarator_a , numarator_b , l , l1 , p;

    numarator_b = ptr_b;
    while (numarator_b != NULL) {
        numarator_b = numarator_b->next;
        lungime_b++;
    }



    if (cars_no == 0) {
        return;
    }
    if (ptr_b != NULL && lungime_b>=0 && pos_b>lungime_b+1) {
        return;
    }
    if (lungime_b == 0 && pos_b>1) {
        return;
    }
    if ( pos_b < 1) {
        return;
    }



    if (ptr_a != NULL) {
        numarator_a = ptr_a;
        while (numarator_a != NULL) {
            numarator_a = numarator_a->next;
            lungime_a++;
        }
        if (cars_no>lungime_a) {
            return;
        }
        if (lungime_a>pos_a && lungime_a-pos_a < cars_no-1) {
            return;
        }
        if (pos_a>1) {
            while (ptr_a!=NULL && contor_a < pos_a) {
                prev_a = ptr_a;
                ptr_a = ptr_a->next;
                contor_a++;
            }
            aux = ptr_a;
            while (aux != NULL && cont_aux < cars_no) {
                aux=aux->next;
                cont_aux++;
            }
            prev_a->next = aux;
        } else {
            aux=statie->platforms[platform_a]->train_cars;
            while (aux!=NULL && cont_aux < cars_no) {
                aux=aux->next;
                cont_aux++;
            }
            statie->platforms[platform_a]->train_cars = aux;
        }
    }
    move_sec = ptr_a;

    if (ptr_b!=NULL) {
        if (cars_no == 1) {
            if (pos_b == 1) {
                move_sec->next=ptr_b;
                ptr_b = move_sec;
                statie->platforms[platform_b]->train_cars=ptr_b;
            } else {
                while (ptr_b != NULL && contor_b < pos_b) {
                    prev = ptr_b;
                    ptr_b = ptr_b->next;
                    contor_b++;
                }
                prev->next = move_sec;
                move_sec->next = ptr_b;
            }
        } else {
            if (pos_b == 1) {
                tmp = move_sec;
                while (tmp!=NULL && c<cars_no) {
                    tmp=tmp->next;
                    c++;
                }
                tmp->next = ptr_b;
                ptr_b = move_sec;
                statie->platforms[platform_b]->train_cars=ptr_b;
            } else {
                while (ptr_b != NULL && contor_b < pos_b && contor_b<=lungime_b) {
                    prev = ptr_b;
                        ptr_b = ptr_b->next;
                        contor_b++;
                }
                while (prev != NULL && a<cars_no) {
                    prev->next = move_sec;
                    move_sec=move_sec->next;
                    prev=prev->next;
                    a++;
                }
                move_sec->next=ptr_b;
            }
        }
    } else {
        if (cars_no == 1) {
            prev = NULL;
            helper = move_sec;
            helper->next = NULL;
            statie->platforms[platform_b]->train_cars = helper;
        } else {
            tmp = move_sec;
            while (tmp!=NULL && c<cars_no) {
                tmp=tmp->next;
                c++;
            }
            tmp->next = ptr_b;
            ptr_b = move_sec;
            statie->platforms[platform_b]->train_cars=ptr_b;
        }
    }
}


/* Gaseste trenul cel mai rapid.
 *
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_express_train(TrainStation *station) {
    TTrainStation statie = station;
    TTrainCar vagoane;
    int greutate = 0 , diff , max = 0 , peron;
    if (statie != NULL) {
        for (int i = 0; i < statie->platforms_no; ++i) {
            vagoane = statie->platforms[i]->train_cars;
            greutate = 0;
            diff = 0;
            while (vagoane != NULL) {
                greutate += vagoane->weight;
                vagoane = vagoane->next;
            }
            diff = statie->platforms[i]->locomotive_power - greutate;
            if (diff >= max) {
                max = diff;
                peron = i;
            }
        }
    }
    return peron;
}


/* Gaseste trenul supraincarcat.
 *
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_overload_train(TrainStation *station) {
    TTrainStation statie = station;
    TTrainCar vagoane;
    int greutate = 0 , peron = -1;
    for (int i = 0; i < station->platforms_no; ++i) {
        if (station->platforms[i]->locomotive_power != -1) {
            vagoane = statie->platforms[i]->train_cars;
            greutate = 0 ;
            while (vagoane != NULL) {
                greutate += vagoane->weight;
                vagoane = vagoane->next;
            }
            if (statie->platforms[i]->locomotive_power < greutate) {
                peron = i;
            }
        }
    }
    if (peron != -1){
        return peron;
    } else {
        return -1;
    }

}


/* Gaseste trenul cu incarcatura optima.
 *
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_optimal_train(TrainStation *station) {
    TTrainStation statie = station;
    TTrainCar vagoane;
    int greutate = 0 , diff = 0 , min = -1 , peron;
    for (int i = 0; i < statie->platforms_no; ++i) {
        vagoane = statie->platforms[i]->train_cars;
        greutate = 0;
        diff = 0;
        if (statie->platforms[i]->locomotive_power != -1) {
            while (vagoane != NULL) {
                greutate += vagoane->weight;
                vagoane = vagoane->next;
            }
            diff = statie->platforms[i]->locomotive_power - greutate;
            if (min == -1) {
                min = diff;
                peron = i;
            }
            if ( min > diff) {
                min = diff;
                peron = i;
            }
        }
    }
    return peron;
}


/* Gaseste trenul cu incarcatura nedistribuita bine.
 *
 * station: gara existenta
 * cars_no: lungimea secventei de vagoane
 *
 * return: peronul pe care se afla trenul
 */
int find_heaviest_sequence_train(TrainStation *station, int cars_no, TrainCar **start_car) {
    TTrainStation statie = station;
    TTrainCar vagoane , aux , tmp , q , prev;
    TTrainCar t = NULL;
    int contor = 1 , greutate_vag_secv_cont , greutate_max = 0 , peron = -1, lungime=0 , dim = 0 , dim_max=0;
    for (int i = 0; i < statie->platforms_no; ++i) {
        q = statie->platforms[i]->train_cars;
        dim = 0;
        while (q!=NULL) {
            q=q->next;
            dim++;
        }
        if (dim_max<dim){
            dim_max=dim;
        }
    }
    if (dim_max<cars_no) {
        *start_car = NULL;
        return -1;
    }
    for (int i = 0; i < statie->platforms_no; ++i) {
        aux = statie->platforms[i]->train_cars;
        while (aux != NULL) {
            tmp = aux;
            greutate_vag_secv_cont = 0;
            contor=1;
            while (tmp!=NULL && contor<=cars_no){
                greutate_vag_secv_cont += tmp->weight;
                prev=tmp;
                tmp=tmp->next;
                contor++;
                if (greutate_vag_secv_cont>greutate_max) {
                    t = aux;
                    greutate_max = greutate_vag_secv_cont;
                    peron=i;
                }
            }
            aux = aux->next;
        }
    }
    *start_car = t;
    return peron;
}


/* Ordoneaza vagoanele dintr-un tren in ordinea descrescatoare a greutatilor.
 *
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 */

void order_train(TrainStation *station, int platform) {
    int contor , nodeContor , aux ,lungime = 0;
    TTrainCar node , current , next , lun;
    node = station->platforms[platform]->train_cars;
    lun = node;
    while (lun != NULL) {
        lun = lun->next;
        lungime++;
    }
    if (lungime == 2) {
        current = node;
        next = node->next;
        if (current->weight < next->weight) {
            next->next = current;
            current->next = NULL;
            station->platforms[platform]->train_cars = next;
        }
    }
    if (lungime > 2) {
        for (nodeContor = lungime - 1; nodeContor >= 0  ; nodeContor--) {
            current = node;
            next = current->next;
            for (contor = 0; contor < nodeContor; ++contor) {
                if (current->weight <= next->weight) {
                    aux = current->weight;
                    current->weight=next->weight;
                    next->weight = aux;
                }
                current = next;
                next = next->next;
            }
        }
    }
}


/* Scoate un vagon din trenul supraincarcat.
 *
 * station: gara existenta
 */
void fix_overload_train(TrainStation *station) {
    int peron = find_overload_train(station) , greutate = 0 , greutate_de_eleim , gasit = 0 , tmp=1  , poz_vag_de_elim , pos = 1 , poz;
    TTrainCar aux , aux1 , vag_de_elim , prev , vag_elim2 ;
    printf("--------------------------\nperonul cu probleme : %d \n" , peron);
    if (peron != -1) {
        aux = station->platforms[peron]->train_cars;
        while (aux!=NULL) {
            greutate += aux->weight;
            aux=aux->next;
        }
        printf("\nputere locomotiva : %d    greutate vagoane : %d\n" , station->platforms[peron]->locomotive_power , greutate);

        greutate_de_eleim = greutate - station->platforms[peron]->locomotive_power;
        printf("GREUTATE DE ELIMINAT : %d\n\n" , greutate_de_eleim);

        aux1 = station->platforms[peron]->train_cars;
        while (aux1!=NULL) {
            if (greutate_de_eleim == aux1->weight) {
                vag_de_elim = aux1;
                gasit = 1;
                poz_vag_de_elim = tmp;
                printf("greutate vad de elim - %d \n " , vag_de_elim->weight);
            } else {
                prev = aux1;
                printf("greutate prev vad de elim - %d \n" , prev->weight);
            }
            aux1 = aux1->next;
            tmp++;
        }
        printf("poz vag de elim %d \n" , poz_vag_de_elim);
        printf("greutate vad de elim - %d \n " , vag_de_elim->weight);
        printf("greutate prev vad de elim - %d \n" , prev->weight);
        if (gasit == 1) {
            if (poz_vag_de_elim > 1) {
                prev->next = vag_de_elim->next;
                free(vag_de_elim);
            }
            if (poz_vag_de_elim == 1) {
                station->platforms[peron]->train_cars = vag_de_elim->next;
                free(vag_de_elim);
            }
        } else {
            TTrainCar aux2;
            while (gasit == 0) {
                pos = 1;
                aux2 = station->platforms[peron]->train_cars;
                greutate_de_eleim = greutate_de_eleim+1;
                while (aux2 != NULL && gasit == 0) {
                    if (greutate_de_eleim == aux2->weight) {
                        gasit = 1;
                        poz = pos;
                        vag_elim2 = aux2;
                    }
                    aux2 = aux2->next;
                    pos++;
                }
            }
            if (poz == 1) {
                station->platforms[peron]->train_cars = vag_elim2->next;
                free(vag_elim2);
            } else {

            }
            printf("-----pos vag de elim dupa cautare = %d----------\n" , poz);
        }
    }
}
