#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define ROWS 1000
#define THREADS 2

// Speichert alle Threaddaten um sie zu übergeben
struct thread_struct{
  int id;
  int** matrix;
  int** resMatrix;
  int* calc; // Speichert alle "Pärchen" von Zeilen und Spalten, die multipliziert werden müssen
  int size;
  int sizeCalc;
  int numThreads; //Threadanzahl
};



int** allocateMatrix(int pSize){
  int** matrix = malloc(pSize*sizeof(int*));
  for(int i=0; i<pSize; i++){
    matrix[i] = malloc(pSize*sizeof(int));
  }
  return matrix;
}

void fillRandomMatrix(int** pMatrix, int pSize){
  for(int a=0; a<pSize; a++){
    for(int b=0; b<pSize; b++){
      pMatrix[a][b] = rand() % 10;
    }
  }
}

void fillZeroMatrix(int** pMatrix, int pSize){
  for(int a=0; a<pSize; a++){
    for(int b=0; b<pSize; b++){
      pMatrix[a][b] = 0;
    }
  }
}

void printMatrix(int** pMatrix, int pSize){
  for(int a=0; a<pSize; a++){
    for(int b=0; b<pSize; b++){
      printf("%d   ",pMatrix[a][b]);
    }
    printf("\n");
  }
}
//Zeile X wird mit Spalte Y multipliziert
void multiplyRowCol(int** pMatrix, int** resMatrix, int pSize, int pRow, int pCol){
  for(int a=0; a<pSize; a++){
    resMatrix[pRow][pCol] += pMatrix[pRow][a]*pMatrix[a][pCol];
  }
}

//Aufruf der einzelnen Multiplikationen um die ganze Matrix auszurechnen
void performCalculation(int pId, int** pMatrix, int** resMatrix, int pSize, int* calc, int pSizeCalc, int numThreads){
  for(int i=pId*2; i<pSizeCalc; i+= 2 *numThreads){
    multiplyRowCol(pMatrix, resMatrix, pSize, calc[i], calc[i+1]);
    }
}


void* worker(void* thread_str_ptr){
  struct thread_struct* data = (struct thread_struct*) thread_str_ptr;
  performCalculation(data->id, data->matrix, data->resMatrix, data->size, data->calc, data->sizeCalc, data->numThreads);
  return 0;
}


//Alle einzelnen Daten werden in ein Struct gespeichert 
struct thread_struct* createThreadData(int pId, int** pMatrix, int** resMatrix, int pSize, int* calc, int pSizeCalc, int num_Threads){
  struct thread_struct* helpThread= malloc(sizeof(struct thread_struct));
 helpThread->matrix = pMatrix;
 helpThread->resMatrix = resMatrix;
 helpThread->size = pSize;
 helpThread->calc = calc;
 helpThread->sizeCalc = pSizeCalc;
 helpThread->id=pId;
 helpThread->numThreads = num_Threads;
 return helpThread;
}

void threading(int** pMatrix, int** resMatrix, int pSize, int numThreads){
  int* helpCalc = malloc(sizeof(int)*pSize*pSize*2);
  //Array von Pärchen ZeilexSpalt wird erzeugt
  for(int i=0,c=0; i<pSize; i++){
    for(int a=0; a<pSize; a++){
      helpCalc[c]=i;
      helpCalc[c+1]=a;
      c+=2;
    }
  }
  pthread_t* threads = malloc(sizeof(pthread_t) * numThreads);
  for(int x=0; x<numThreads; x++){
    struct thread_struct* helpData = createThreadData(x,pMatrix, resMatrix, pSize, helpCalc, pSize*pSize*2, numThreads);
    //worker(helpData);
    if(pthread_create(&threads[x], NULL, (void*) worker, helpData)){
      printf("Failed %d",x);
    }
  }
  for(int y=0; y<numThreads; y++){
    if(pthread_join(threads[y],NULL)){
      printf("Doooof bei %d \n",y);
    }
  }
}




int main(){
  int** matrix = allocateMatrix(ROWS);
  int** resMatrix = allocateMatrix(ROWS);
  fillRandomMatrix(matrix,ROWS);
  fillZeroMatrix(resMatrix,ROWS);
  //printMatrix(matrix,ROWS);
  //printMatrix(resMatrix,ROWS);
  threading(matrix, resMatrix,ROWS,THREADS);
 // multiplyRowCol(matrix,resMatrix,ROWS,0,0);
 // printMatrix(resMatrix,ROWS);
  return 0;
}

