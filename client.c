#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "helpers.h"
#include "parson.h"
#include "requests.h"

char *cookie;
char *token;

// Functie pentru a scoate cookie ul dintr-un raspuns
char *extract_cookie(char *response) {
  char *start = strstr(response, "Set-Cookie:");
  if (!start) return NULL;
  start += strlen("Set-Cookie:");
  while (*start == ' ') start++;
  char *end = strchr(start, ';');
  int len = end - start;
  char *ret = calloc(len + 1, 1);
  memcpy(ret, start, len);
  return ret;
}

// Functie pentru a
void get_movies(void) {
  if (!token) {
    printf("ERROR: Nu aveți acces la library!\n");
    return;
  }
  char *auth = malloc(strlen(token) + 30);
  sprintf(auth, "Authorization: Bearer %s", token);
  char ck[256];
  if (cookie) sprintf(ck, "Cookie: %s", cookie);

  char request[1000];
  sprintf(request,
          "GET /api/v1/tema/library/movies HTTP/1.1\r\n"
          "Host: 63.32.125.183\r\n"
          "%s\r\n"
          "%s\r\n"
          "Connection: close\r\n\r\n",
          auth, ck);

  int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockfd, request);
  char *resp = receive_from_server(sockfd);

  if (strstr(resp, "200 OK")) {
    printf("SUCCESS: Lista filmelor\n");
    char *json = basic_extract_json_response(resp);
    JSON_Value *v = json_parse_string(json);
    JSON_Array *a = json_object_get_array(json_value_get_object(v), "movies");
    for (size_t i = 0; i < json_array_get_count(a); ++i) {
      JSON_Object *o = json_array_get_object(a, i);
      printf("#%d %s\n", (int)json_object_get_number(o, "id"),
             json_object_get_string(o, "title"));
    }
  } else {
    printf("ERROR: get_movies eșuat!\n");
  }
  close(sockfd);
}

void get_movie(void) {
  int id;
  if (!token) {
    printf("ERROR: Nu aveți acces la library!\n");
    return;
  }
  printf("id=");
  scanf("%d", &id);

  char uri[100];
  sprintf(uri, "/api/v1/tema/library/movies/%d", id);
  char *auth = malloc(strlen(token) + 30);
  sprintf(auth, "Authorization: Bearer %s", token);
  char ck[256];
  if (cookie) sprintf(ck, "Cookie: %s", cookie);

  char request[1000];
  sprintf(request,
          "GET %s HTTP/1.1\r\nHost: 63.32.125.183\r\n%s\r\n%s\r\nConnection: "
          "close\r\n\r\n",
          uri, auth, ck);

  int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockfd, request);
  char *resp = receive_from_server(sockfd);

  if (strstr(resp, "200 OK")) {
    printf("SUCCESS: Detalii film\n");
    char *json = basic_extract_json_response(resp);
    JSON_Value *v = json_parse_string(json);
    JSON_Object *o = json_value_get_object(v);
    printf("title: %s\n", json_object_get_string(o, "title"));
    printf("year: %d\n", (int)json_object_get_number(o, "year"));
    printf("description: %s\n", json_object_get_string(o, "description"));
    JSON_Value *doub = json_object_get_value(o, "rating");
    printf("rating: %.1f\n", atof(json_value_get_string(doub)));
  } else {
    printf("ERROR: get_movie eșuat!\n");
  }
  close(sockfd);
}

void add_movie(void) {
  if (!token) {
    printf("ERROR: Nu aveți acces la library!\n");
    return;
  }
  char title[256], descriere[256];
  int year;
  double rating;
  printf("title=");
  scanf(" %[^\n]", title);
  printf("year=");
  scanf("%d", &year);
  printf("description=");
  scanf("  %[^\n]", descriere);
  printf("rating=");
  scanf("%lf", &rating);

  JSON_Value *v = json_value_init_object();
  JSON_Object *o = json_value_get_object(v);
  json_object_set_string(o, "title", title);
  json_object_set_number(o, "year", year);
  json_object_set_string(o, "description", descriere);
  json_object_set_number(o, "rating", rating);
  char *body = json_serialize_to_string(v);

  char *auth = malloc(strlen(token) + 30);
  sprintf(auth, "Authorization: Bearer %s", token);
  char ck[256];
  if (cookie) sprintf(ck, "Cookie: %s", cookie);

  char request[1000];
  sprintf(request,
          "POST /api/v1/tema/library/movies HTTP/1.1\r\n"
          "Host: 63.32.125.183\r\n%s\r\n%s\r\n"
          "Content-Type: application/json\r\nContent-Length: %ld\r\n"
          "Connection: close\r\n\r\n%s",
          auth, ck, strlen(body), body);

  int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockfd, request);
  char *resp = receive_from_server(sockfd);

  if (strstr(resp, "201 CREATED") || strstr(resp, "200 OK"))
    printf("SUCCESS: Film adăugat\n");
  else
    printf("ERROR: add_movie eșuat!\n");

  close(sockfd);
}

void update_movie(void) {
  int id;
  if (!token) {
    printf("ERROR: Nu aveți acces la library!\n");
    return;
  }
  printf("id=");
  scanf("%d", &id);
  char title[256], descriere[256];
  int year;
  double rating;
  printf("title=");
  scanf("  %[^\n]", title);
  printf("year=");
  scanf("%d", &year);
  printf("description=");
  scanf("  %[^\n]", descriere);
  printf("rating=");
  scanf("%lf", &rating);

  JSON_Value *v = json_value_init_object();
  JSON_Object *o = json_value_get_object(v);
  json_object_set_string(o, "title", title);
  json_object_set_number(o, "year", year);
  json_object_set_string(o, "description", descriere);
  json_object_set_number(o, "rating", rating);
  char *body = json_serialize_to_string(v);

  char uri[100];
  sprintf(uri, "/api/v1/tema/library/movies/%d", id);
  char *auth = malloc(strlen(token) + 30);
  sprintf(auth, "Authorization: Bearer %s", token);
  char request[1000];
  sprintf(request,
          "PUT %s HTTP/1.1\r\nHost: 63.32.125.183\r\n%s\r\n"
          "Content-Type: application/json\r\nContent-Length: %ld\r\n"
          "Connection: close\r\n\r\n%s",
          uri, auth, strlen(body), body);

  int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockfd, request);
  char *resp = receive_from_server(sockfd);

  if (strstr(resp, "200 OK"))
    printf("SUCCESS: Film actualizat\n");
  else
    printf("ERROR: update_movie eșuat!\n");

  close(sockfd);
}

void delete_movie(void) {
  int id;
  if (!token) {
    printf("ERROR: Nu aveți acces la library!\n");
    return;
  }
  printf("id=");
  scanf("%d", &id);

  char uri[100];
  sprintf(uri, "/api/v1/tema/library/movies/%d", id);
  char *auth = malloc(strlen(token) + 30);
  sprintf(auth, "Authorization: Bearer %s", token);
  char request[1000];
  sprintf(request,
          "DELETE %s HTTP/1.1\r\nHost: 63.32.125.183\r\n%s\r\nConnection: "
          "close\r\n\r\n",
          uri, auth);

  int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
  send_to_server(sockfd, request);
  char *resp = receive_from_server(sockfd);

  if (strstr(resp, "200 OK"))
    printf("SUCCESS: Film șters cu succes\n");
  else
    printf("ERROR: delete_movie eșuat!\n");

  close(sockfd);
}

int main(void) {
  char command[32];
  while (1) {
    scanf("%s", command);
    if (!strcmp(command, "login_admin")) {
      char user[100], pass[100];
      printf("username=");
      scanf("%s", user);
      printf("password=");
      scanf("%s", pass);

      JSON_Value *v = json_value_init_object();
      JSON_Object *o = json_value_get_object(v);
      json_object_set_string(o, "username", user);
      json_object_set_string(o, "password", pass);
      char *body = json_serialize_to_string(v);

      char *msg =
          compute_post_request("63.32.125.183", "/api/v1/tema/admin/login",
                               "application/json", &body, 1, NULL, 0);
      int sockfd =
          open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
      send_to_server(sockfd, msg);
      char *resp = receive_from_server(sockfd);

      if (strstr(resp, "200 OK")) {
        cookie = extract_cookie(resp);
        printf("SUCCESS: Admin logat cu succes!\n");
      } else {
        printf("ERROR: Autentificare eșuată!\n");
      }

      close(sockfd);
    } else if (!strcmp(command, "add_user")) {
      if (!cookie) {
        printf("ERROR: Trebuie să fii logat ca admin.\n");
        continue;
      }
      char user[100], pass[100];
      printf("username=");
      scanf("%s", user);
      printf("password=");
      scanf("%s", pass);

      JSON_Value *v = json_value_init_object();
      JSON_Object *o = json_value_get_object(v);
      json_object_set_string(o, "username", user);
      json_object_set_string(o, "password", pass);
      char *body = json_serialize_to_string(v);

      char *cookies[100] = {cookie};
      char *msg =
          compute_post_request("63.32.125.183", "/api/v1/tema/admin/users",
                               "application/json", &body, 1, cookies, 1);

      int sockfd =
          open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
      send_to_server(sockfd, msg);
      char *resp = receive_from_server(sockfd);

      if (strstr(resp, "201 CREATED") || strstr(resp, "200 OK"))
        printf("SUCCESS: Utilizator adăugat cu succes!\n");
      else
        printf("ERROR: Nu s-a putut adăuga utilizatorul!\n");

      close(sockfd);
    } else if (!strcmp(command, "get_users")) {
      if (!cookie) {
        printf("ERROR: Trebuie să fii logat ca admin.\n");
        continue;
      }
      char *cookies[100] = {cookie};
      char *msg = compute_get_request(
          "63.32.125.183", "/api/v1/tema/admin/users", NULL, cookies, 1);

      int sockfd =
          open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
      send_to_server(sockfd, msg);
      char *resp = receive_from_server(sockfd);

      if (strstr(resp, "200 OK")) {
        printf("SUCCESS: Lista utilizatorilor\n");
        char *json = basic_extract_json_response(resp);
        JSON_Value *v = json_parse_string(json);
        JSON_Array *a =
            json_object_get_array(json_value_get_object(v), "users");
        for (size_t i = 0; i < json_array_get_count(a); ++i) {
          JSON_Object *u = json_array_get_object(a, i);
          printf("#%zu %s:%s\n", i + 1, json_object_get_string(u, "username"),
                 json_object_get_string(u, "password"));
        }
      } else {
        printf("ERROR: Nu s-au putut obține utilizatorii!\n");
      }

      close(sockfd);
    } else if (!strcmp(command, "login")) {
      if (cookie) {
        printf("ERROR: Deja autentificat!\n");
        continue;
      }
      char admin_user[100], user[100], pass[100];
      printf("admin_username=");
      scanf("%s", admin_user);
      printf("username=");
      scanf("%s", user);
      printf("password=");
      scanf("%s", pass);

      JSON_Value *v = json_value_init_object();
      JSON_Object *o = json_value_get_object(v);
      json_object_set_string(o, "admin_username", admin_user);
      json_object_set_string(o, "username", user);
      json_object_set_string(o, "password", pass);
      char *body = json_serialize_to_string(v);

      char *msg =
          compute_post_request("63.32.125.183", "/api/v1/tema/user/login",
                               "application/json", &body, 1, NULL, 0);
      int sockfd =
          open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
      send_to_server(sockfd, msg);
      char *resp = receive_from_server(sockfd);

      if (strstr(resp, "200 OK")) {
        cookie = extract_cookie(resp);
        printf("SUCCESS: Autentificare reușită\n");
      } else {
        printf("ERROR: Autentificare eșuată!\n");
      }
      close(sockfd);
    } else if (!strcmp(command, "logout")) {
      cookie = NULL;
      token = NULL;
      printf("SUCCESS: Utilizator delogat\n");
    } else if (!strcmp(command, "logout_admin")) {
      cookie = NULL;
      token = NULL;
      printf("SUCCESS: Admin delogat\n");
    } else if (!strcmp(command, "get_access")) {
      if (!cookie) {
        printf("ERROR: Trebuie să fii autentificat!\n");
      } else {
        char *cookies[100] = {cookie};
        char *request = compute_get_request(
            "63.32.125.183", "/api/v1/tema/library/access", NULL, cookies, 1);

        int sockfd =
            open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
        send_to_server(sockfd, request);
        char *resp = receive_from_server(sockfd);

        if (strstr(resp, "200 OK")) {
          char *json_start = basic_extract_json_response(resp);

          JSON_Value *val = json_parse_string(json_start);

          JSON_Object *obj = json_value_get_object(val);
          const char *t = json_object_get_string(obj, "token");

          char *tok = malloc(500);
          strcpy(tok, t);
          token = tok;
          printf("SUCCESS: Token JWT primit\n");
        } else {
          printf("ERROR: Nu s-a putut obține acces!\n");
        }
        close(sockfd);
      }
    } else if (!strcmp(command, "get_movies"))
      get_movies();
    else if (!strcmp(command, "get_movie"))
      get_movie();
    else if (!strcmp(command, "add_movie"))
      add_movie();
    else if (!strcmp(command, "update_movie"))
      update_movie();
    else if (!strcmp(command, "delete_movie"))
      delete_movie();
    else if (!strcmp(command, "exit"))
      break;
  }
  return 0;
}
