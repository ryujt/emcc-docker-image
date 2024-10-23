# emcc-docker-image

이 프로젝트는 **Emscripten**을 기반으로 하여 C/C++ 코드를 WebAssembly로 컴파일하기 위한 Docker 환경을 제공합니다. Docker 컨테이너 내부에서 Emscripten 개발 환경을 쉽게 구축할 수 있으며, WebAssembly 파일을 컴파일하고 테스트할 수 있습니다.

## Docker 이미지 생성 및 컨테이너 실행

1. **Docker 이미지 생성**
   - 아래 명령어는 Dockerfile을 기반으로 `emcc`라는 이름의 이미지를 생성합니다.
   ```bash
   docker build -t emcc -f docker/ubuntu-24.04.1-emcc .
   ```

2. **Docker 컨테이너 실행**
   - `docker run` 명령을 통해 `emcc-container`라는 이름의 컨테이너를 실행합니다. 로컬 디렉토리 `/Users/ryu/projects/`를 컨테이너의 `/root/projects/` 경로에 마운트하며, 포트 8080을 외부에 노출합니다.
   ```bash
   docker run -d -p 8080:8080 -v /Users/ryu/projects/:/root/projects/ --name emcc-container emcc
   ```

3. **Zsh로 컨테이너 접속 및 환경 설정**
   - 실행 중인 컨테이너에 접속하고 Emscripten 환경 변수를 설정한 후 Zsh 쉘을 실행합니다.
   ```bash
   docker exec -it emcc-container zsh -c "source /root/emsdk/emsdk_env.sh && /bin/zsh"
   ```

## 예제 빌드 및 테스트 #1

### 간단한 예제 빌드

1. **프로젝트 디렉토리 이동**
   - 간단한 C 파일을 WebAssembly로 컴파일하기 위한 예제 프로젝트 디렉토리로 이동합니다.
   ```bash
   cd ~/projects/personal/emcc-docker-image/examples/simple-example
   ```

2. **C 파일을 WebAssembly로 컴파일**
   - `add.c` 파일을 Emscripten을 사용해 WebAssembly로 컴파일합니다. 컴파일 결과로 `add.js`와 `add.wasm` 파일이 생성됩니다.
   ```bash
   emcc add.c -s WASM=1 -s EXPORTED_FUNCTIONS='["_add"]' -o add.js
   ```

3. **간단한 웹 서버 실행**
   - `http-server`를 설치한 후, 포트 8080에서 간단한 웹 서버를 실행하여 WebAssembly 파일을 테스트할 수 있습니다.
   ```bash
   npm install -g http-server
   http-server -p 8080
   ```

## 예제 빌드 및 테스트 #2

이 섹션에서는 추가적인 외부 라이브러리인 `zlib`을 Emscripten 환경에서 어떻게 빌드하고 사용할 수 있는지 설명합니다. 특히, Emscripten으로 직접 외부 라이브러리를 빌드하여 WebAssembly로 컴파일하는 과정을 다룹니다. 이는 외부 라이브러리를 사용하는 모든 C/C++ 프로젝트에 적용할 수 있는 일반적인 방법입니다.

### zlib 빌드

1. **zlib 라이브러리 다운로드 및 압축 해제**
   - `zlib`은 파일 압축/해제 기능을 제공하는 널리 사용되는 C 라이브러리입니다. 이 라이브러리를 Emscripten 환경에서 WebAssembly로 빌드하기 위해 먼저 소스를 다운로드하고 압축을 해제합니다.
   ```bash
   cd ~/projects/personal/emcc-docker-image/examples/zlib
   wget https://zlib.net/fossils/zlib-1.3.1.tar.gz
   tar -xzvf zlib-1.3.1.tar.gz
   cd zlib-1.3.1
   ```

2. **Emscripten을 사용하여 zlib 빌드**
   - `emconfigure`와 `emmake` 명령어를 사용하여 Emscripten 환경에 맞게 `zlib`을 WebAssembly로 빌드합니다. 여기서 `emconfigure`는 `configure` 스크립트를 Emscripten과 호환되도록 설정하며, `emmake`는 `make` 명령어를 Emscripten으로 빌드할 수 있게 처리합니다.
   - 이 과정은 다른 외부 라이브러리들도 동일한 방식으로 Emscripten을 통해 빌드할 수 있습니다.
   ```bash
   emconfigure ./configure
   emmake make
   ```

   **설명:**
   - `emconfigure ./configure`: Emscripten 환경에 맞춰 `zlib`의 컴파일 설정을 만듭니다.
   - `emmake make`: 설정된 내용을 기반으로 `zlib` 라이브러리를 WebAssembly로 빌드합니다.

   이 명령어들을 실행하면 `zlib` 라이브러리가 Emscripten용으로 빌드되고, WebAssembly로 컴파일할 준비가 됩니다. 빌드된 결과물은 해당 디렉토리에 저장됩니다.

### zlib 사용 예제 빌드

이 단계에서는 앞서 빌드된 `zlib` 라이브러리를 사용하여 `zlib.c` 파일을 WebAssembly로 컴파일합니다. 이 과정에서 Emscripten 컴파일러는 외부 라이브러리(`zlib`)와 해당 헤더 파일들을 참조하여 프로젝트를 빌드합니다.

1. **빌드된 zlib 사용을 위한 환경 변수 설정**
   - 컴파일 시 `zlib` 헤더 파일과 라이브러리 경로를 정확하게 지정해주어야 합니다. 이를 위해 `CPPFLAGS`와 `LDFLAGS` 환경 변수를 설정하여, Emscripten이 `zlib` 헤더 파일 및 라이브러리를 찾을 수 있도록 합니다.
   ```bash
   export CPPFLAGS="-I./zlib-1.3.1"
   export LDFLAGS="-L./zlib-1.3.1"
   ```

   **설명:**
   - `CPPFLAGS="-I./zlib-1.3.1"`: `zlib.h` 헤더 파일이 있는 경로를 지정합니다.
   - `LDFLAGS="-L./zlib-1.3.1"`: 빌드된 `zlib` 라이브러리 파일이 있는 경로를 지정합니다.

2. **WebAssembly 컴파일**
   - 이제 `zlib` 라이브러리를 링크하여 `zlib.c` 파일을 WebAssembly로 컴파일합니다. 이 명령어는 `zlib` 라이브러리를 포함하여 WebAssembly와 자바스크립트 파일을 생성합니다.
   ```bash
   emcc zlib.c -s WASM=1 -s EXPORTED_FUNCTIONS='["_malloc", "_free", "_compress_data", "_decompress_data", "_free_compress_result"]' -s EXPORTED_RUNTIME_METHODS='["stringToUTF8", "UTF8ToString", "getValue"]' -s ALLOW_MEMORY_GROWTH=1 -O3 $CPPFLAGS $LDFLAGS -lz -o zlib.js
   ```

   **설명:**
   - `-s WASM=1`: WebAssembly로 컴파일하도록 지정합니다.
   - `-s EXPORTED_FUNCTIONS`: WebAssembly에서 사용할 함수들을 지정합니다.
   - `-s EXPORTED_RUNTIME_METHODS`: Emscripten 런타임에서 사용할 메서드를 지정합니다.
   - `-O3`: 최적화 옵션을 적용합니다.
   - `$CPPFLAGS $LDFLAGS`: 앞서 설정한 경로를 사용해 `zlib` 헤더와 라이브러리를 참조합니다.
   - `-lz`: `zlib` 라이브러리를 링크합니다.

3. **간단한 웹 서버 실행**
   - WebAssembly 파일이 정상적으로 생성되면 이를 테스트하기 위해 웹 서버를 실행합니다. `http-server`를 설치하고, 포트 8080에서 서버를 실행하여 WebAssembly 파일을 브라우저에서 테스트할 수 있습니다.
   ```bash
   npm install -g http-server
   http-server -p 8080
   ```

### 추가적인 라이브러리 설치 및 사용에 대한 설명

이 예제는 **Emscripten을 사용하여 외부 라이브러리(여기서는 `zlib`)를 WebAssembly로 빌드하고 사용하는 방법**을 설명합니다. 대부분의 C/C++ 프로젝트에서는 `zlib` 외에도 다양한 외부 라이브러리를 사용하는데, 이 과정에서 발생할 수 있는 문제는 Emscripten 환경에 맞게 빌드되지 않았기 때문일 수 있습니다.

**Emscripten 환경에서 외부 라이브러리 사용 시 주의사항:**
- 외부 라이브러리의 소스 코드를 다운로드한 후, Emscripten의 `emconfigure`와 `emmake`를 사용해 빌드해야 합니다. 이는 Emscripten이 WebAssembly로 컴파일할 수 있는 환경을 구성하는 데 필요합니다.
- 라이브러리가 Emscripten의 `sysroot` 디렉토리 내에 존재하지 않으므로, 빌드된 라이브러리와 헤더 파일의 경로를 명시적으로 지정해야 합니다.
- 프로젝트 빌드 시에는 `CPPFLAGS`와 `LDFLAGS`로 해당 경로를 지정한 후, `emcc`를 사용하여 WebAssembly로 컴파일합니다.

이를 통해 Emscripten 환경에서 **다양한 외부 라이브러리**를 프로젝트에 통합하여 WebAssembly 기반의 프로젝트를 구축할 수 있습니다.
