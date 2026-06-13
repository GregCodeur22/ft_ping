# ft_ping

Réimplémentation de la commande `ping` en C, utilisant des raw sockets et le protocole ICMP.

---

## Objectif du projet

Recréer le comportement de base de `/bin/ping` :
- Envoyer des paquets ICMP Echo Request à une cible (IP ou nom de domaine)
- Recevoir les ICMP Echo Reply
- Mesurer le RTT (Round Trip Time) de chaque paquet
- Afficher les statistiques finales à l'interruption (Ctrl+C)

---

## Architecture

```
ft_ping/
├── includes/
│   └── ft_ping.h          # Structures, prototypes, includes système
├── src/
│   ├── main.c             # Boucle principale, gestion signal, stats finales
│   ├── parsing.c          # Parsing des arguments CLI (-n, -w, -s...)
│   ├── parsing_target.c   # Résolution DNS (getaddrinfo)
│   ├── build_paquet.c     # Construction du paquet ICMP + timestamp
│   ├── send_paquet.c      # Envoi via sendto()
│   └── recv_paquet.c      # Réception, calcul RTT, reverse DNS, affichage
└── Makefile
```

---

## Étapes de construction

### 1. Structures de données (`ft_ping.h`)

Deux structures principales :

- **`t_config`** — options passées en ligne de commande (`-n`, `-w`, `-s`, `-v`...)
- **`t_ping`** — état courant de la session (socket, séquence, PID, stats RTT, `start_time`)

```c
typedef struct s_ping {
	struct sockaddr_in addr;
	char   ip_str[16];
	int    socket_fd;
	int    sequence;
	int    pid;
	int    packets_transmitted;
	int    packets_received;
	double rtt_min;
	double rtt_max;
	double rtt_total;
	struct timeval start_time;
} t_ping;
```

---

### 2. Parsing des arguments (`parsing.c`)

Parcourir `argv` et remplir `t_config` :
- Détecter les flags (`-n`, `-w N`, `-s N`...)
- Stocker la cible dans `config.target`
- Valider les valeurs numériques avec `is_number()`

---

### 3. Résolution de la cible (`parsing_target.c`)

Utiliser `getaddrinfo()` pour accepter aussi bien une IP qu'un nom de domaine :

```c
getaddrinfo(target, NULL, &hints, &res);
inet_ntop(AF_INET, &ping->addr.sin_addr, ping->ip_str, sizeof(ping->ip_str));
```

Cela remplit `ping->addr` (utilisé dans `sendto`) et `ping->ip_str` (utilisé dans les `printf`).

---

### 4. Création de la raw socket (`main.c`)

```c
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
```

> Nécessite les droits root (`sudo`) car les raw sockets sont privilégiées.

---

### 5. Construction du paquet ICMP (`build_paquet.c`)

Le paquet fait 64 octets au total :

| Zone         | Taille       | Contenu                        |
|--------------|--------------|--------------------------------|
| En-tête ICMP | 8 octets     | type, code, id, seq, checksum  |
| Timestamp    | 16 octets    | `struct timeval` (pour le RTT) |
| Padding      | 40 octets    | remplissage (`'i'`)            |

Étapes :
1. Forger l'en-tête (`ICMP_ECHO`, id = PID, seq croissant)
2. Injecter le timestamp avec `gettimeofday()` dans le payload
3. Calculer le checksum sur l'intégralité du paquet

---

### 6. Envoi du paquet (`send_paquet.c`)

```c
sendto(ping->socket_fd, buff, paquet_size, 0,
	   (struct sockaddr *)&ping->addr, sizeof(ping->addr));
```

---

### 7. Réception et calcul du RTT (`recv_paquet.c`)

1. Recevoir avec `recvmsg()` dans un buffer alloué dynamiquement
2. Sauter l'en-tête IP (`ip_hl * 4` octets) pour atteindre l'en-tête ICMP
3. Vérifier `type == ICMP_ECHOREPLY` et `id == pid`
4. Extraire le timestamp embarqué dans le payload
5. Calculer le RTT :

```c
double rtt = (tv_recv.tv_sec  - tv_send.tv_sec)  * 1000.0
		   + (tv_recv.tv_usec - tv_send.tv_usec) / 1000.0;
```

6. Mettre à jour `rtt_min`, `rtt_max`, `rtt_total`
7. Résolution DNS inverse avec `getnameinfo()` (affiche `hostname (IP)` si disponible)

---

### 8. Boucle principale et signal (`main.c`)

```c
signal(SIGINT, sig_handler);  // Ctrl+C → g_stop = 1

while (!g_stop) {
	send_paquet(&ping, &config);
	recv_paquet(&ping, &config);
	sleep(1);
}

display_stats(&ping, &config);
```

---

### 9. Statistiques finales (`display_stats`)

Affichées après Ctrl+C :

```
--- google.com ping statistics ---
5 packets transmitted, 5 received, 0% packet loss, time 4003ms
rtt min/avg/max = 10.123/11.456/13.789 ms
```

---

## Compilation et utilisation

```bash
make

sudo ./ft_ping google.com
sudo ./ft_ping 8.8.8.8
sudo ./ft_ping -w 5 google.com   # timeout de 5 secondes
```

> `sudo` est obligatoire pour les raw sockets ICMP.

---

## Concepts clés à maîtriser

| Concept              | Fonction(s) utilisée(s)              |
|----------------------|--------------------------------------|
| Raw socket           | `socket(AF_INET, SOCK_RAW, ...)`     |
| Résolution DNS       | `getaddrinfo()`, `inet_ntop()`       |
| Reverse DNS          | `getnameinfo()` avec `NI_NAMEREQD`   |
| Envoi ICMP           | `sendto()`                           |
| Réception ICMP       | `recvmsg()`                          |
| Timestamp RTT        | `gettimeofday()`, `struct timeval`   |
| Checksum             | Complément à 1 sur 16 bits           |
| Gestion signal       | `signal(SIGINT, handler)`            |


---

## Bonus : `-c` (count)

Arrête automatiquement le ping après N paquets envoyés, sans avoir à faire Ctrl+C.

```bash
sudo ./ft_ping -c 3 google.com
```

### Implémentation

**`t_config`** — ajout du champ `int c` (0 = infini, N = s'arrête après N paquets).

**`parsing.c`** — parsing identique à `-w` : supporte `-c3` et `-c 3`, valide que la valeur est un entier positif.

**`main.c`** — condition de sortie ajoutée en fin de boucle :

```c
while (!g_stop)
{
	send_paquet(&ping, &config);
	ping.packets_transmitted++;
	recv_paquet(&ping, &config);
	ping.sequence++;
	if (config.c && ping.packets_transmitted >= config.c)
		break;
	sleep(1);
}
```

Le `sleep(1)` est placé **après** le check pour ne pas attendre 1 seconde inutilement sur le dernier paquet.

---

## Bugs corrigés

### `-i interval`

Définit le délai en secondes entre chaque paquet envoyé. Par défaut `ping` attend 1 seconde entre chaque envoi. Utile pour ralentir le ping (diagnostic réseau discret) ou pour tester la stabilité sur une longue durée sans flood le réseau.

```bash
sudo ./ft_ping -i 2 google.com        # 1 paquet toutes les 2 secondes
sudo ./ft_ping -c 5 -i 3 google.com   # 5 paquets, 3 secondes entre chaque
```

**Implémentation :**

- `int i` ajouté dans `t_config`, initialisé à `1` (comportement par défaut inchangé)
- Parsing identique à `-c` et `-w` dans `parsing.c`
- `sleep(1)` remplacé par `sleep(config.i)` dans la boucle de `main.c`

---

### `-q` quiet

Supprime l'affichage ligne par ligne de chaque paquet reçu. Seuls le header initial et les statistiques finales sont affichés. Utile quand on veut juste le résultat global sans le flood de lignes.

```bash
sudo ./ft_ping -q google.com
sudo ./ft_ping -q -c 10 google.com
```

**Implémentation :**

- `int q` ajouté dans `t_config`, initialisé à `0`
- `-q` est un flag simple (pas de valeur) dans `parsing.c`
- Les deux `printf` par paquet dans `recv_paquet.c` sont enveloppés dans `if (!config->q)`

---

### `-t TTL`

Définit le TTL (Time To Live) des paquets sortants. Chaque routeur traversé décrémente le TTL de 1. Quand il atteint 0, le routeur détruit le paquet et renvoie un `ICMP Time Exceeded`. Utile pour limiter la portée des paquets ou simuler le comportement de `traceroute`.

```bash
sudo ./ft_ping -t 64 google.com    # TTL normal
sudo ./ft_ping -t 3 google.com     # les paquets meurent en route → Time to live exceeded
```

**Implémentation :**

- `int t` ajouté dans `t_config`, initialisé à `0` (0 = ne pas modifier le TTL par défaut du système)
- Valeur validée entre 1 et 255 dans `parsing.c`
- Appliqué après création du socket dans `main.c` via `setsockopt` :

```c
setsockopt(ping.socket_fd, IPPROTO_IP, IP_TTL, &config.t, sizeof(config.t));
```

---

### Bug 2 — `ICMP_TIME_EXCEEDED` affiché seulement en mode `-v`

**Problème :** Quand un paquet expire en route (TTL trop petit), le routeur renvoie un `ICMP_TIME_EXCEEDED`. Le code ne l'affichait que si `-v` était actif — résultat : avec `-t 4` le programme ressemblait à `-q`, rien ne s'affichait par paquet.

Deux erreurs supplémentaires dans le même bloc :
- L'IP affichée était celle de la **cible** (`ping->ip_str`) au lieu du **routeur** qui a renvoyé l'erreur
- Si le paquet reçu n'était pas le nôtre (PID différent), on faisait `return` au lieu de `continue` — la boucle `while(1)` s'arrêtait alors qu'elle aurait dû réessayer

**Fix :**

```c
// IP du routeur qui a renvoyé l'erreur (pas la cible)
char from_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &from.sin_addr, from_ip, sizeof(from_ip));

// Affiché toujours (sauf -q), pas seulement en -v
if (!config->q)
	printf("From %s: icmp_seq=%d Time to live exceeded\n", from_ip, seq);

// PID non correspondant → on réessaie
if (ntohs(inner_icmp->un.echo.id) != ping->pid)
	continue;  // était: return
```

**Fichier :** `src/recv_paquet.c`

---

### Bug 1 — `recv_paquet` ne boucle pas sur les paquets non pertinents

**Problème :** `recv_paquet` appelait `recvmsg()` une seule fois. Si le raw socket recevait un paquet ICMP d'un autre processus (ID différent du nôtre), la fonction retournait sans compter le paquet, et la vraie réponse restait bloquée dans le buffer. Résultat avec `-c 2` : 2 paquets transmis, 1 reçu.

**Fix :** `recv_paquet` boucle maintenant sur `recvmsg()` jusqu'à obtenir un paquet ICMP Echo Reply dont l'ID correspond à notre PID, ou jusqu'à une erreur.

```c
while (1)
{
	bytes_received = recvmsg(ping->socket_fd, &msg, 0);
	if (bytes_received < 0)
		return;
	// ...
	if (icmp_hdr->type == ICMP_ECHOREPLY && ntohs(icmp_hdr->un.echo.id) != ping->pid)
		continue;  // paquet d'un autre process, on réessaie
	// ...
	return;  // paquet reçu et compté
}
```

**Fichier :** `src/recv_paquet.c`