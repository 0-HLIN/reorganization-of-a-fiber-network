CFLAGS = -Wall -g
CC = gcc


ChaineMain: ChaineMain.o Chaine.o SVGwriter.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
ReconstitueReseau: ReconstitueReseau.o Chaine.o SVGwriter.o Reseau.o Hachage.o ArbreQuat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
complexite: complexite.o Chaine.o SVGwriter.o Reseau.o Hachage.o ArbreQuat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
comparaison_temps_cpu: comparaison_temps_cpu.o Chaine.o SVGwriter.o Reseau.o Hachage.o ArbreQuat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
test_graphe: test_graphe.o Graphe.o Struct_File.o Struct_Liste.o Chaine.o SVGwriter.o Reseau.o Hachage.h ArbreQuat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
test_hachage: test_hachage.o Hachage.o Reseau.o SVGwriter.o
	$(CC) $(CFLAGS) -o $@ $^

############# les .o #############

test_graphe.o: test_graphe.c Graphe.h SVGwriter.h Chaine.h ArbreQuat.h Hachage.h Reseau.h
	$(CC) $(CFLAGS) -o $@ -c $<
test_hachage.o: test_hachage.c Hachage.h
	$(CC) $(CFLAGS) -o $@ -c $<

complexite.o: complexite.c Chaine.h Reseau.h Hachage.h ArbreQuat.h
	$(CC) $(CFLAGS) -o $@ -c $<
comparaison_temps_cpu.o: comparaison_temps_cpu.c  Chaine.h Reseau.h Hachage.h ArbreQuat.h
	$(CC) $(CFLAGS) -o $@ -c $<

ChaineMain.o: ChaineMain.c Chaine.h
	$(CC) $(CFLAGS) -o $@ -c $<
ReconstitueReseau.o: ReconstitueReseau.c Chaine.h Reseau.h Hachage.h ArbreQuat.h
	$(CC) $(CFLAGS) -o $@ -c $<


Graphe.o: Graphe.c Graphe.h Reseau.h
	$(CC) $(CFLAGS) -o $@ -c $<
Struct_List.o: Struct_List.c Struct_List.h
	$(CC) $(CFLAGS) -o $@ -c $<
Struct_File.o: Struct_File.c Struct_File.h
	$(CC) $(CFLAGS) -o $@ -c $<


Hachage.o: Hachage.c Hachage.h Reseau.h Chaine.h
	$(CC) $(CFLAGS) -o $@ -c $<
ArbreQuat.o: ArbreQuat.c ArbreQuat.h Reseau.h Chaine.h
	$(CC) $(CFLAGS) -o $@ -c $<
Reseau.o: Reseau.c Reseau.h Chaine.h SVGwriter.h
	$(CC) $(CFLAGS) -o $@ -c $<

Chaine.o: Chaine.c SVGwriter.c Chaine.h SVGwriter.h
	$(CC) $(CFLAGS) -o $@ -c $<
SVGwriter.o: SVGwriter.c SVGwriter.h
	$(CC) $(CFLAGS) -o $@ -c $<



clean:
	rm -f *.o
