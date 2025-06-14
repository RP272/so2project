## Problem jedzących filozofów
"Jedzą, piją, lulki palą, tańce, hulanka, swawola" - A. Mickiewicz lub Młody Gdynianin

W oryginalnym problemie jedzących filozofów pięciu filozofów siedzi przy stole. Każdy filozof może robić jedną z trzech czynności: myśleć, jeść lub być głodnym. Na stole znajduje się pięc widelców (pałeczek). Każdy filozof ma swoją miskę z jedzeniem, jednakże aby przystąpić do konsumpcji musi mieć oba widelce, które się przy nim znajdują. Przy założeniu, że w jednym momencie filozof bierze tylko jeden widelec do ręki, może nastąpić tak zwany deadlock. Jeżeli w tym samym czasie każdy filozof pochwyci widelec znajdujący się po jego lewej stronie, żaden z nich nie będzie mógł uzyskać drugiego widelca. W takiej sytuacji filozofowie będą oczekiwali w nieskończoność. 
Sytuacja ta znajduje odzwierciedlenie w temacie wielowątkowości.
W informatyce istnieje pojęcie warunków koniecznych do wystąpienia blokady:
1. Wzajemne wykluczenie (mutual exclusion): w przypadku filozofów, żaden widelec nie może być używany jednocześnie przez kilka osób.
2. Przetrzymywanie i oczekiwanie ( resource holding): filozofowie trzymają jeden widelec w oczekiwaniu na drugi.
3. Brak wywłasczeń (no preemption): filozof nie może wyrwać z ręki widelca innemu filozofowi.
4. Cykl w oczekiwaniu (circular wait): istnieje pętla, w której każdy z filozofów czeka na zwolnienie zasobu przez filozofa po swojej lewej stronie. 

Eliminacja jednego z czterech powyższych warunków eliminuje możliwość wystąpienia nieskończonego oczekiwania (deadlock'a). 
Rozwiązanie zaimplementowane w programie wyklucza warunek numer 2. Rozwiązanie to zostało opracowane przez osobę, która pierwotnie sformułowała problem, a mianowicie Edsgera Dijkstrę. Zgodnie z jego założeniem, filozofowie w jednym momencie podnoszą oba widelce. 

W rozwiązaniu użyto zarówno mutexów jak i semaforów. Dwa mutexy wykorzystywane są w celu ograniczenia dostępu do sekcji krytycznej, którymi są stany filozofów oraz standardowy strumień wyjścia. 
Semafory z kolei przedstawiają stan, w którym dany filozof przejął oba widelce. Jeżeli filozof przy użyciu funkcji .acquire() nie zablokował się, to oznacza, że udało mu sie przejąc oba widelce. 
Kluczową ideą rozwiązania, która zapobiega zagłodzeniu wątków (filozofów) jest fakt, że filozof po odłożeniu widelców, sprawdza czy jego sąsiedzi mogą jeść. 

Program został wykonany w środowisku Visual Studio 2022. W celu uruchomienia projektu należy pobrać repozytorium.
W folderze jedzacy-filozofowie/DiningPhilosophers/DiningPhilosophers znajduje się plik DiningPhilosophers.cpp
Należy przejść do tego folderu i użyć komendy g++ w następującej postaci:
g++ -std=c++20 DiningPhilosophers.cpp -o DiningPhilosophers.
Aby następnie uruchomić program:
./DiningPhilosophers

Źródła:
https://en.wikipedia.org/wiki/Dining_philosophers_problem
http://smurf.mimuw.edu.pl/node/955
## Wielowątkowy serwer czatu
Programy umożliwiają uruchomienie serwera czatu działającego na określonym porcie. Program klienta umożliwia połączenie się z serwerem oraz wysyłanie wiadomości do członków czatu. Oprócz tego klient odbiera wiadomości wysyłane przez pozostałych użytkowników. Kod został podzielony na dwa pliki: client.cpp oraz server.cpp. Kod serwera czatu oparty jest o mechanizm socket-ów, które pozwalają na komunikację między procesami za pomocą protokołu TCP/IP. Na serwerze tworzony jest socket, do którego przypisywany jest adres oraz port. Następnie serwer nasłuchuje na tym sockecie w nieskończonej pętli while na połączenia przychodzące ze strony klienta. Dla każdego nowego klienta tworzony jest osobny wątek, który uruchamia funkcję handle_clnt. Funkcja handle_clnt wysyła wiadomości od klientów do pozostałych klientów oraz obsługuje opuszczenie czatu przez klienta. Funkcja wykorzystuje mechanizm mutexów w celu modyfikacji mapy przechowującej sockety klientów oraz licznika klientów. 

Kod programu klienta realizuje kilka funkcjonalności. Po pierwsze tworzy socket dla klienta, a następnie nawiązuje połączenie z socketem serwera. W kodzie klienta zdefiniowano dwie funkcje: send_msg do wysylania wiadomości do serwera oraz recv_msg, która odbiera komunikaty użytkowników wysyłane z serwera. Dzięki użyciu wielowątkowości oraz mechanizmu mutex serwer może obsługiwać wielu użytkowników jednocześnie. 

### Synchronizacja
Dla każdego klienta tworzony jest osobny wątek oraz socket. Sockety przechowywane są w strukturze mapy, gdzie kluczem jest nazwa użytkownika a wartością deskryptor socketu. W funkcji uruchomionej przez każdy wątek dostęp do mapy odbywa się w sekcji krytycznej z użyciem mechanizmu mutex. 

Historia wiadomości czatu nie jest przechowywana na serwerze. Wiadomości przychodzące ze strony klienta rozsyłane są do wszystkich pozostałych klientów (broadcast).

### Uruchomienie programu
W folderze wielowatkowy-chat znajdują się pliki client.cpp oraz server.cpp. W celu kompilacji plików do programów wykonywalnych należy użyć komendy: 
<br>g++ -std=c++20 server.cpp -o server
<br>g++ -std=c++20 client.cpp -o client<br>
W celu uruchomienia serwera należy użyć komendy: ./server<br>
Aby uruchomić klienta należy użyć komendy: ./client NAZWA_UZYTKOWNIKA<br>



