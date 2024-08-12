# Server API

- [AsyncWebServer](#asyncwebserver)
  - [Get ESP32 Info](#get-esp32-info-get-post)
  - [Set ESP32 STA SSID and Password](#set-esp32-sta-ssid-and-password-get-post)
  - [Set ESP32 AP SSID and Password](#set-esp32-ap-ssid-and-password-get-post)
  - [Set ESP32 mDNS and Hostname](#set-esp32-mdns-and-hostname-get-post)
- [AsyncWebSocket](#asyncwebsocket)

## AsyncWebServer

## Get ESP32 Info `GET` `POST`

Get ESP32 information

### `Headers` For ESP32 Info

- `Content-Type: application/json`

### `Path` For ESP32 Info

- **URL:** `/api/info`

### `HTTP` For ESP32 Info

- **status codes:**
  - `200` on success
  - `500` on nvs data error

- **Request example:**

```js
fetch('/api/info', {
    method: 'GET',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

- **Response example:**

```json
{
    "sta_ssid": "ssid",
    "sta_password": "password",
    "ap_ssid": "3D Scanner",
    "ap_password": "password",
    "hostname": "3d-scanner",
}
```

## Set ESP32 STA SSID and Password `GET` `POST`

Set ESP32 STA SSID and Password

### `Headers` For ESP32 STA SSID and Password

- `Content-Type: application/json`

### `Path` For ESP32 STA SSID and Password

- **URL:** `/api/set/sta`

### `HTTP` For ESP32 STA SSID and Password

- **status codes:**
  - `200` on success
  - `400` on send data error

- **Request example:**

`GET`:

```js
fetch('/api/set/sta?ssid=<ssid>&password=<password>', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

`POST`:

```js
fetch('/api/set/sta', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
    body: "ssid=<ssid>&password=<password>",
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

- **Response example:**

```json
{
    "code": "200",
    "status": "ok",
    "path": "/api/set/sta",
}
```

```json
{
    "code": "400",
    "status": "error",
    "path": "/api/set/ap",
}
```

## Set ESP32 AP SSID and Password `GET` `POST`

Set ESP32 AP SSID and Password

### `Headers` For ESP32 AP SSID and Password

- `Content-Type: application/json`

### `Path` For ESP32 AP SSID and Password

- **URL:** `/api/ap`

### `HTTP` For ESP32 AP SSID and Password

- **status codes:**
  - `200` on success
  - `400` on send data error

- **Request example:**

`GET`:

```js
fetch('/api/set/ap?ssid=<ssid>&password=<password>', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

`POST`:

```js
fetch('/api/set/ap', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
    body: "ssid=<ssid>&password=<password>",
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

- **Response example:**

```json
{
    "code": "200",
    "status": "ok",
    "path": "/api/set/ap",
}
```

```json
{
    "code": "400",
    "status": "error",
    "path": "/api/set/ap",
}
```

## Set ESP32 mDNS and Hostname `GET` `POST`

Set ESP32 hostname

### `Headers` For ESP32 mDNS and Hostname

- `Content-Type: application/json`

### `Path` For ESP32 mDNS and Hostname

- **URL:** `/api/set/mdns`

### `HTTP` For ESP32 mDNS and Hostname

- **status codes:**
  - `200` on success
  - `400` on send data error

- **Request example:**

`GET`:

```js

fetch('/api/set/mdns?hostname=<hostname>', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

`POST`:

```js
fetch('/api/set/mdns', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
    body: "hostname=<hostname>",
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

- **Response example:**

```json
{
    "code": "200",
    "status": "ok",
    "path": "/api/set/mdns",
}
```

```json
{
    "code": "400",
    "status": "error",
    "path": "/api/set/mdns",
}
```

## Set 3D Scanner status `GET` `POST`

Set 3D Scanner status

### `Headers` For 3D Scanner status

- `Content-Type: application/json`

### `Path` For 3D Scanner status

- **URL:** `/api/set/scanner`

### `HTTP` For 3D Scanner status

- **status codes:**
  - `200` on success
  - `400` on send data error

- **Request example:**

`GET`:

```js
fetch('/api/set/scanner?status=<status>', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

`POST`:

```js
fetch('/api/set/scanner', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
    body: "status=<status>",
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

## AsyncWebSocket

`data`:

```json
{
    "name": "3d-1",
    "points_count": 1,
    "is_last": false,
    "points": [
        [x, y, z]
    ]
}
```
