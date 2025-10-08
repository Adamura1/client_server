# Lotan Roberto-Gabriel

## Descriere

Acest proiect implementeaza un client care comunica cu un server pentru a gestiona niste utilizatori si o bibliotecă de filme.


# Client-Server Movie Library

Client C care comunică cu un server REST pentru gestionarea unei biblioteci de filme.

## Funcționalități
- `register`, `login`, `logout`
- `get_movies`, `get_movie <id>`
- `add_movie`, `delete_movie`, `delete_user`
- Suport cookie + JWT
- Parsare JSON cu [Parson](https://github.com/kgabis/parson)

## Compilare & Rulare

```bash
make
./client
````

## Structură

* `client.c` – logica principală
* `helpers.c/.h`, `requests.c/.h` – funcții auxiliare + cereri HTTP
* `parson.c/.h` – parsare JSON
* `Makefile` – build rapid

