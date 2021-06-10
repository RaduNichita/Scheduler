Nume: Nichita Radu\
Grupă: 331CA

# Tema 4 Planificator de threaduri

## Organizare

```bash
student@so:~$ tree .
.
├── GNUmakefile
├── Makefile
├── data_structures
│   ├── priority_queue.c
│   └── vector.c
├── include
│   ├── comparators.h
│   ├── priority_queue.h
│   ├── so_scheduler.h
│   ├── so_thread.h
│   ├── utils.h
│   └── vector.h
├── so_scheduler.c
├── utils
│   ├── comparators.c
│   └── utils.c
└── wrappers
    ├── lin
    │   └── so_thread_wrapper_lin.c
    └── win
        └── so_thread_wrapper_win.c
```

Am ales această organizare, întrucât am participat la hackathon-ul organizat de echipa de SO [1] și am considerat util să am o organizare de genul pentru viitoarele teme, pentru a le realiza cât mai modular. De asemenea, din acel schelet am învățat cum să fac wrappere astfel încât să arate bine codul. Meersi, echipa de Hackathon SO. :)

Revenind, organizarea este facută astfel încât totul să fie generic, singura diferență între Linux și Windows fiind fișierul wrapper care se compilează.

## Implementare

### Flow

Thread-ul main poate face fork si isi creeaza anumite thread-uri folosind functia so_fork. Acestea sunt planificate astfel incat sa ruleze doar un singur (pe langa eventual cel main la un moment dat).

Pentru asta s-a folosit un semafor cu numar initial 0 pe care s-a dat lock iar abia apoi se creea thread-ul respectiv. Cand se intra in functia asociata thread-ului respectiv (simulată prin folosirea unei functii wrapper), se da lock pe semaforul asociat thread-ului. De aceea, el se va bloca pana semaforul asociat acestuia va fi deblocat de planificator.

Astfel, functia de planificare (sau planificatorul) va fi apelat dupa fiecare dintre operațiile majore:
* so-fork
* so-wait
* so-signal
* so-exec

Planificatorul va alegere thread-ul cu prioritatea cea mai mare din cele care sunt în starea READY. Pentru a nu avea probleme cu ordinea din coada de prioritate în caz de egalitate, am introdus un timestamp global al sistemului. Un thread va primi pe lângă prioritate, și un timp asociat. Acesta va fi incrementat doar la momentul adăugării în coadă a unor elemente.

Accessul la planificator este exclusiv pentru a evita situatia urmatoare, de exemplu:

`MAIN TH` - `so_fork`\
`TH1` -`so_fork`\
`MAIN` - `so_fork`

In acest caz doua threaduri ar incerca sa adauge in acelasi moment de timp in priority queue) - pentru fiecare functie teoretic putem avea o problema similara, întrucât în temă vor rula două threaduri la un moment dat (thread-ul main si thread-ul care va rula confrom planificatorului).

Funcția reschedule alegere care este următorul thread ce va fi deblocat astfel:

* dacă nu rulează niciun thread sau dacă thread-ul care rulează s-a terminat, atunci se va alege următorul thread din coada. Dacă nu există niciun thread în coadă, înseamnă că aceasta este goală, deci toate thread-urile s-au terminat, astfel se notifică so-end.

* dacă thread-ul care rulează a venit din stare waiting, atunci el va fi preemtat și va rula următorul thread din coada de priorități.

* dacă thread-ul care rulează este încă în starea running, dar i s-a terminat cuanta de timp alocată, lui i se va permite din nou accesul la procesor în caz că e singurul rămas sau dacă nu există un alt thread cu o prioritate mai mare în coadă. Altfel, el va fi preemptat și se va adăuga în pq cu un nou timpstamp, conform precizărilor de mai sus. O altă soluție ar fi fost să fie ținute un număr de (SO_MAX-PRIO-SO_MIN_PRIO) cozi de prioritate, câte una pentru fiecare prioritate. În acest caz s-ar fi consumat semnificativ aceeași memorie aproximativ, dar inserția ar fi putut să fie efectuată în timp constant vs logaritmic în implementarea cu pq.

La finalul funcției de reschedule, se preemptează thread-ul care a rulat până în acel moment, dacă acest lucru este necesar.

### Precizări

* enunțul temei a fost complet implementat, realizându-se funcții wrapper peste majoritatea celor din pthread și din WinAPI.

* implementarea de vector și priority queue au fost făcute să semene cu cele din C++. (la fel și implementarea parțială de vector iterator, mă gândeam că o să folosesc for-each dar nu a fost cazul).

## Feedback

* cea mai mișto temă de până acum la SO, cred că am înțeles mai bine cum se aleg anumite thread-uri care rulează pe procesor. A fost și o mare provocare această temă, prin complexitatea ei aș putea spune că a fost în top3 teme grele de la ACS de până acum (Tema5 SO says hi, cel mai probabil).
* Să ne vedem sănătoși și pentru README-ul de la tema5.

## Compilare și rulare

`Linux`- ```bash make -f GNUmakefile```
`Windows` - ```bash nmake -f Makefile```
Se va creea o bibliotecă care va putea fi ulterită folosită.

Bibliografie

[1] https://ocw.cs.pub.ro/courses/so/teme/hackathon

[2] The Linux Programming Interface

[3] Cursurile de planificare și thread-uri SO

[4] Laburile de SO de thread-uri (8 + 9)

