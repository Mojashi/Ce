import "stl/Integer.ce"
    
Integer:(8) x,y,z;
void main(){
    1 <= x*z + y;
    x*z + y <= 100;
    
    40 <= y;
    y <= 60;
    
    x%3 = 0;
    z*z = 100;
    
    [-x.overflow];
    [-y.overflow];
    [-z.overflow];
}