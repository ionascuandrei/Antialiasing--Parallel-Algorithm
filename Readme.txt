Ionascu Andrei

		Part 1
	Pentru pastrarea informatiilor fisierului de intrare am folosit
structura image ce contine int-uri dedicate tipului, inaltimii,
latimii, valorii maxime a imaginii si un vector de stringuri in
care salvez pixelii sub forma de unsigned char.
	Pentru transmiterea datelor legate de input si output catre functia
threadFunction am folosit structura threads_info ce adauga si id-ul thread-ului
curent.
	Pentru imagini color, campul image din structura image va fi construit
sub forma: R1 G1 B1 R2 G2 B2; iar pentru imagini alb-negru: Pixel1 Pixel2;
	Scalabilitatea se realizeaza prin impartirea imaginii pe linii in functie
de numarul de thread-uri, fara a exista coliziune. Fiecare thread lucreaza
in intervalul startLine-endLine avand intreaga linie de pixeli pana la
width.
	Pentru creerea imaginii de output, programul parcurge fiecare pixel
din output si ii creeaza noua valoare folosindu-se de pixelii din imaginea
de input din intervalul:
	(lineIN, columnIN) - (lineIN+resizeFactor, columnIn+resizeFactor)
	
	
		Part 2
	Pentru fiecare pixel se calculeaza distanta pana la linia oferita,
tinand cont de mijlocul fiecarui pixel (+0.5) si dimensiunea imaginii
(*100/frame->resolution)
	Scalabilitatea se realizeaza la fel ca si la exercitiul anterior,
fiecare thread lucrand pe linii diferite din imagine.