# emcc-docker-image

이 프로젝트는 **Emscripten**을 기반으로 하여 C/C++ 코드를 WebAssembly로 컴파일하기 위한 Docker 환경을 제공합니다. Docker 컨테이너 내부에서 Emscripten 개발 환경을 쉽게 구축할 수 있으며, WebAssembly 파일을 컴파일하고 테스트할 수 있습니다.

컴퓨터에 Docker가 설치되어 있다고 가정하고 설명을 이어가겠습니다. Docker 설치 및 사용법에 대한 자세한 내용은 다른 자료를 참고하시기 바랍니다.

## Docker 이미지 생성 및 컨테이너 실행

1. **Docker 이미지 생성**
   - 미리 작성된 Dockerfile을 기반으로 `emcc`라는 이름의 이미지를 생성합니다.
   ```bash
   docker build -t emcc -f docker/ubuntu-24.04.1-emcc .
   ```

2. **Docker 컨테이너 생성**
   - `docker run` 명령을 통해 `emcc-container`라는 이름의 컨테이너를 생성합니다. 이때 호스트 컴퓨터의 프로젝트 폴더를 컨테이너 내부에 마운트하여 Docker 안에서도 동일한 폴더를 사용할 수 있도록 설정합니다.
   ```bash
   docker create -p 8080:8080 -v /your/host/projects/:/root/projects/ --name emcc-container emcc
   ```
   - **참고:** `-v` 옵션에서 `/your/host/projects/` 부분을 여러분의 실제 프로젝트 경로로 변경하십시오. 이 옵션을 사용하지 않으면 Docker 내부에서 소스 코드를 직접 다운로드하여 사용할 수 있습니다.

3. **Docker 컨테이너 시작**
   ```bash
   docker start emcc-container
   ```

4. **컨테이너 접속 및 Emscripten 환경 설정**
   - 컨테이너에 접속하면서 Emscripten 환경 변수가 설정되도록 스크립트를 실행합니다. 아래 명령어를 그대로 사용하시면 됩니다.
   ```bash
   docker exec -it emcc-container zsh -c "source /root/emsdk/emsdk_env.sh && /bin/zsh"
   ```
   - **설명:** 이 명령어는 컨테이너에 접속한 후 Emscripten 환경 설정을 하고 Zsh 쉘을 실행합니다.

## 예제 빌드 및 테스트 #1

### 간단한 예제 빌드

1. **프로젝트 디렉토리 이동**
   - 간단한 C 파일을 WebAssembly로 컴파일하기 위한 예제 프로젝트 디렉토리로 이동합니다.
   ```bash
   cd ~/projects/personal/emcc-docker-image/examples/simple-example
   ```
   - **참고:** 경로는 여러분의 프로젝트 구조에 따라 변경될 수 있습니다.

2. **C 파일을 WebAssembly로 컴파일**
   - `add.c` 파일을 Emscripten을 사용하여 WebAssembly로 컴파일합니다. 이 과정에서 `add.js`와 `add.wasm` 파일이 생성됩니다.
   ```bash
   emcc add.c -s WASM=1 -s EXPORTED_FUNCTIONS='["_add"]' -o add.js
   ```
   - **설명:**
     - `-s EXPORTED_FUNCTIONS='["_add"]'`: JavaScript에서 사용할 수 있도록 내보낼 C 함수 `_add`를 지정합니다.

3. **간단한 웹 서버 실행**
   - WebAssembly 파일을 테스트하기 위해 간단한 웹 서버를 실행합니다. `http-server`를 설치하고 포트 8080에서 서버를 시작합니다.
   ```bash
   npm install -g http-server
   http-server -p 8080
   ```
   - **참고:** `http-server`는 Node.js 기반의 간단한 웹 서버입니다.

4. **웹 브라우저에서 테스트**
   - 웹 브라우저를 열고 `http://localhost:8080`에 접속하여 생성된 WebAssembly 파일의 동작을 확인합니다.

### 추가 설명

- **사용된 C 코드**
  - 예제의 `add.c`는 두 개의 숫자를 더해주는 간단한 함수입니다. 기존의 C 코드를 WebAssembly로 컴파일하여 웹에서 사용할 수 있습니다.

- **AI를 활용한 개발**
  - 기존의 C 코드를 인공지능에게 전달하여 WebAssembly로 빌드할 수 있게 수정하고, 빌드 스크립트나 테스트용 HTML 코드도 AI를 통해 작성할 수 있습니다. 이러한 작업을 통해 개발자는 반복적인 작업을 줄이고 결과물 검증에 집중할 수 있습니다.

## 예제 빌드 및 테스트 #2

이 섹션에서는 외부 라이브러리인 `zlib`을 Emscripten 환경에서 빌드하고 사용하는 방법을 설명합니다. 이는 외부 라이브러리를 사용하는 모든 C/C++ 프로젝트에 적용할 수 있는 일반적인 방법입니다.

### zlib 빌드

1. **zlib 라이브러리 다운로드 및 압축 해제**
   - 파일 압축/해제 기능을 제공하는 `zlib` 라이브러리를 다운로드하고 압축을 해제합니다.
   ```bash
   cd ~/projects/personal/emcc-docker-image/examples/zlib
   wget https://zlib.net/fossils/zlib-1.3.1.tar.gz
   tar -xzvf zlib-1.3.1.tar.gz
   cd zlib-1.3.1
   ```

2. **Emscripten을 사용하여 zlib 빌드**
   - `emconfigure`와 `emmake`를 사용하여 Emscripten 환경에 맞게 `zlib`을 빌드합니다.
   ```bash
   emconfigure ./configure
   emmake make
   ```
   - **설명:**
     - `emconfigure`: `configure` 스크립트를 Emscripten과 호환되도록 설정합니다.
     - `emmake`: `make` 명령어를 Emscripten으로 빌드할 수 있게 처리합니다.

### zlib 사용 예제 빌드

1. **환경 변수 설정**
   - 컴파일 시 `zlib` 헤더 파일과 라이브러리 경로를 지정합니다.
   ```bash
   export CPPFLAGS="-I./zlib-1.3.1"
   export LDFLAGS="-L./zlib-1.3.1"
   ```

2. **WebAssembly로 컴파일**
   - `zlib.c` 파일을 컴파일하여 `zlib.js`와 `zlib.wasm` 파일을 생성합니다.
   ```bash
   emcc zlib.c -s WASM=1 -s EXPORTED_FUNCTIONS='["_malloc", "_free", "_compress_data", "_decompress_data", "_free_compress_result"]' -s EXPORTED_RUNTIME_METHODS='["stringToUTF8", "UTF8ToString", "getValue"]' -s ALLOW_MEMORY_GROWTH=1 -O3 $CPPFLAGS $LDFLAGS -lz -o zlib.js
   ```
   - **설명:**
     - `-lz`: `zlib` 라이브러리를 링크합니다.
     - `$CPPFLAGS`와 `$LDFLAGS`: 앞서 설정한 헤더 파일 및 라이브러리 경로를 사용합니다.
     - `-s EXPORTED_FUNCTIONS`: WebAssembly에서 사용할 함수들을 지정합니다.
     - `-s EXPORTED_RUNTIME_METHODS`: Emscripten 런타임에서 사용할 메서드를 지정합니다.

3. **간단한 웹 서버 실행**
   - 생성된 WebAssembly 파일을 테스트하기 위해 웹 서버를 실행합니다.
   ```bash
   http-server -p 8080
   ```

### 추가적인 라이브러리 설치 및 사용에 대한 설명

- **외부 라이브러리 사용 시 주의사항**
  - Emscripten 환경에서 외부 라이브러리를 사용할 때는 해당 라이브러리를 Emscripten으로 빌드해야 합니다.
  - 라이브러리가 Emscripten의 `sysroot` 디렉토리에 존재하지 않으므로, 헤더 파일과 라이브러리 경로를 명시적으로 지정해야 합니다.
  - `emconfigure`와 `emmake`를 사용하여 라이브러리를 빌드하고, 컴파일 시 경로를 지정합니다.

이를 통해 Emscripten 환경에서 다양한 외부 라이브러리를 사용하는 WebAssembly 프로젝트를 구축할 수 있습니다.