gradient : L1 ou L2 ?
gestion des bords : approximation circulaire, zero-padding ?


[réduction du bruit]
En chaque point :
Gradient horizontal : hgrad [taille de l'image] = u(x+1,y) - u(x-1,y)
Gradient vertical : vgrad [taille de l'image] = u(x,y+1) - u(y,y-1))
Gradient : abs(grad) = abs(vgrad) + abs(hgrad) = abs(u(x+1,y) - u(x-1,y)) + abs(u(x,y+1) - u(y,y-1))
Direction du gradient = arctan(hgrad/vgrad)

On recherche les maxima de abs(grad)

//u(x,y,nx,ny) = img(x*ny + y);


unsigned char* mask = malloc(sizeof(unsigned char) * nx * ny);

//Valeur du gradient :

double hgrad,vgrad;

for(x = 0 ; x < nx ; x++) {
	for(y = 0 ; y < ny ; y++) {
		//Gradient horizontal
		hgrad = hgrad[x*ny + y] = (x==0) ? (data[ny + y] - data[[nx-1]*ny + y]) : ((x==nx-1) ? (data[y] - data[[nx-2]*ny + y]) : (data[[x+1]*ny + y] - data[[x-1]*ny + y]));
		//Gradient vertical
		vgrad = (y==0) ? (data[x*ny + 1] - data[x*ny + ny-1]) : ((y==ny-1) ? (data[x*ny] - data[x*ny + ny-2]) : (data[x*ny + y+1 ] - data[x*ny + y-1]));
		//Norme (L1) du gradient
		grad[x*ny + y] = fabs(hgrad) + fabs(vgrad); 
		//Direction du gradient
		// si vgrad est à zero :
		// hgrad neg : -pi/2 sinon pi/2
		// automatiquement fait par atan <3
		//Mais on veut pas (sinon on a pas -pi/4
		theta[x*ny + y] = floor((M_PI_2 + atan(hgrad / vgrad))/M_PI_4); 
	}
}

//Suppression des non-maximums

for(x = 0 ; x < nx ; x++) {
	for(y = 0 ; y < ny ; y++) {
		double past, future;
		int t = theta[x*ny + y];
		switch(t) {
			case 0:
			case 4: 
				past = data[x*ny + y-1];
				future = data[x*ny + y+1];
				break;
			case 1:
				future = data[[x+1]*ny + y+1];
				past = data[[x-1]*ny + y-1];
				break;
			case 2:	
				future = data[[x+1]*ny + y];
				past = data[[x-1]*ny + y];
				break;
			case 3:
				future = data[x+1,y-1];
				past = data[x-1,y+1]
				past
				break;
			default: printf("Une couille dans le potage");
				return;
				break;
		}	
		double present = data[x*ny + y];
		mask[x * ny + y] = (present < past) ? 0 : ((present < future) ? 0 : 1);
	}
}
/*
gradient : entre -pi/2 et pi/2

atan + pi/2 entre 0 et pi 
(a + pi/2)/(pi/4) entre 0 et 4 :
si = à 0 ou 4 : 0 ou pi
si = 1 : pi/4
si = 2 : pi/2
si = 3 : 3*pi/4
*/
