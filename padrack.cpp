#include "padrack.h"

int padrack:: getQuantity(){
    return quantity;
}
int padrack:: setQuantity(int number){
    quantity = number;
}
int padrack:: decQuantity(){
    if(quantity != 0){
        quantity--;
    }
}
void padrack:: setZero(){
     if(quantity = 0){
        quantity = 0;
    }
}