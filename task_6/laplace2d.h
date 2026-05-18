void initialize(double *__restrict A, double *__restrict Anew, int m, int n);

double calcNext(double *__restrict A, double *__restrict Anew, int m, int n);
        
void swap(double *&__restrict A, double *&__restrict Anew);

void deallocate(double *__restrict A, double *__restrict Anew);