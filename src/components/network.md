# Server API

- [AsyncWebServer](#asyncwebserver)
  - [Get ESP32 Info](#get-esp32-info-get)
  - [Set ESP32 Data](#set-esp32-data-get)
  - [Set 3D Scanner status](#set-3d-scanner-status-get)
- [AsyncWebSocket](#asyncwebsocket)
  - [Request data](#request-data)
  - [Response data](#response-data)
    - [When Stop to setting mode](#when-stop-to-setting-mode)

## AsyncWebServer

## Get ESP32 Info `GET`

Get ESP32 information

### `Headers` For ESP32 Info

- `Content-Type: application/json`

### `Path` For ESP32 Info

- **URL:** `/api/info`

### `HTTP` For ESP32 Info

- **status codes:**
  - `200` on success
  - `500` on Server error

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
    "code": "200",
    "status": "ok",
    "path": "/api/info",
    "data": {
        "mdns": "3d-scanner",
        "sta": {
            "ssid": "ssid",
            "password": "password",
        },
        "ap": {
            "ssid": "3D Scanner",
            "password": "password",
        },
        "github": {
            "username": "MakerbaseMoon",
            "repo": "3d_scanner_esp",
        },
        "module": {
            "z_axis_max": 47000,
            "z_axis_start_step": 0,
            "z_axis_delay_time": 100,
            "z_axis_one_time_step": 100,
            "x_y_axis_max": 6400,
            "x_y_axis_step_delay_time": 100,
            "x_y_axis_one_time_step":32,
            "vl53l1x_center": 110,
            "vl53l1x_timeing_budget": 15,
        }
    }
}
```

```json
{
    "code": "500",
    "status": "Server Error",
    "path": "/api/info",
}
```

## Set ESP32 Data `GET`

Set ESP32 STA SSID and Password, ESP32 AP SSID and Password, ESP32 mDNS and Hostname, 3D Scanner status

### `Headers` For ESP32 STA SSID and Password

- `Content-Type: application/json`

### `Path` For ESP32 STA SSID and Password

- **URL:** `/api/set/data`

### `HTTP` For ESP32 STA SSID and Password

- **status codes:**
  - `200` on success
  - `500` on Server error

- **Request Param:**

- `sta_ssid`:
  - Type: String
  - Note: STA SSID
  - Need: `sta_password`
- `sta_password`:
  - Type: String
  - Note: STA Password
  - Need: `sta_ssid`
- `ap_ssid`:
  - Type: String
  - Note: AP SSID
  - Need: `ap_password`
- `ap_password`:
  - Type: String
  - Note: AP Password
  - Need: `ap_ssid`
- `mdns`:
  - Type: String
  - Note: ESP32 mDNS and Hostname
- `github_username`:
  - Type: String
  - Note: Github username
  - Need: `github_repo`
- `github_repo`:
  - Type: String
  - Note: Github repo
  - Need: `github_username`
- `z_axis_max`:
  - Type: Number
  - Note: Z axis max
  - Need: **ALL Module Setting**
- `z_axis_start_step`:
  - Type: Number
  - Note: Z axis start step
  - Need: **ALL Module Setting**
- `z_axis_delay_time`:
  - Type: Number
  - Note: Z axis delay time
  - Need: **ALL Module Setting**
- `z_axis_one_time_step`:
  - Type: Number
  - Note: Z axis one time step
  - Need: **ALL Module Setting**
- `x_y_axis_max`:
  - Type: Number
  - Note: X Y axis max
  - Need: **ALL Module Setting**
- `x_y_axis_step_delay_time`:
  - Type: Number
  - Note: X Y axis step delay time
  - Need: **ALL Module Setting**
- `x_y_axis_one_time_step`:
  - Type: Number
  - Note: X Y axis one time step
  - Need: **ALL Module Setting**
- `vl53l1x_center`:
  - Type: Number
  - Note: VL53L1X center
  - Need: **ALL Module Setting**
- `vl53l1x_timeing_budget`:
  - Type: Number
  - Note: VL53L1X timeing budget
  - Need: **ALL Module Setting**

- **Request example:**

`GET`:

```js
fetch('/api/set/data?sta_ssid=<sta_ssid>&sta_password=<sta_password>&ap_ssid=<ap_ssid>&ap_password=<ap_password>&mdns=<mdns>&github_username=<github_username>&github_repo=<github_repo>&z_axis_max=<z_axis_max>&z_axis_start_step=<z_axis_start_step>&z_axis_delay_time=<z_axis_delay_time>&z_axis_one_time_step=<z_axis_one_time_step>&x_y_axis_max=<x_y_axis_max>&x_y_axis_step_delay_time=<x_y_axis_step_delay_time>&x_y_axis_one_time_step=<x_y_axis_one_time_step>&vl53l1x_center=<vl53l1x_center>&vl53l1x_timeing_budget=<vl53l1x_timeing_budget>', {
    method: 'GET',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

## Set 3D Scanner status `GET`

Set 3D Scanner status

### `Headers` For 3D Scanner status

- `Content-Type: application/json`

### `Path` For 3D Scanner status

- **URL:** `/api/set/scanner`

### `HTTP` For 3D Scanner status

- **status codes:**
  - `200` on success
  - `400` on command or param error
  - `500` on Server error

- **Request Param:**

- `command`:
  - Type: String
  - Note: 3D Scanner status
  - Value: `home`, `new`, `start`, `stop`, `end`, `up`, `down`, `left`, `right`

- Value for `new`:
  - `name`:
    - Type: String
    - Note: 3D Scanner name

- Value for `up`, `down`, `left`, `right`:
  - `step`:
    - Type: Number
    - Note: 3D Scanner step
    - default: 1

- **Request example:**

`GET`:

```js
fetch('/api/set/scanner?command=<command>', {
    method: 'GET',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

`New`
  
```js
fetch('/api/set/scanner?command=new&name=<name>', {
    method: 'GET',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

`Up`, `Down`, `Left`, `Right`

```js
fetch('/api/set/scanner?command=<command>&step=<step>', {
    method: 'GET',
    headers: {
        'Content-Type': 'application/json',
    },
})
.then((response) => response.json())
.then((data) => console.log(data))
.catch((error) => console.error(error));
```

## AsyncWebSocket

### `Request data`

- `command`:
  - Type: String
  - Note: 3D Scanner status
  - Value: `home`, `new`, `start`, `stop`, `end`, `up`, `down`, `left`, `right`

- Value for `new`:
  - `name`:
    - Type: String
    - Note: 3D Scanner name

- Value for `up`, `down`, `left`, `right`:
  - `step`:
    - Type: Number
    - Note: 3D Scanner step
    - default: 1

```json
{
    "command": "home",
}
```

```json
{
    "command": "new",
    "name": "3d-1",
}
```

```json
{
    "command": "up",
    "step": 10000,
}
```

### `Response data`

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

### When Stop to setting mode

```json
{
    "z_steps": 0,
    "vl53l1x": 0,
}
```
