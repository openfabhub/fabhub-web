# Builder Image
FROM alpine:3.11 AS builder

ENV PYTHONUNBUFFERED=1

RUN apk update && apk add --no-cache \
  build-base \
  cmake \
  g++ \
  gcc \
  git \
  libstdc++ \
  linux-headers \
  python3 \
  py-pip && \
  if [ ! -e /usr/bin/python ]; then ln -sf python3 /usr/bin/python ; fi && \
  python3 -m ensurepip && \
  rm -r /usr/lib/python*/ensurepip && \
  pip3 install --no-cache --upgrade pip setuptools wheel && \
  if [ ! -e /usr/bin/pip ]; then ln -s pip3 /usr/bin/pip ; fi && \
  pip --no-cache-dir install conan && \
  conan user

# Build Environment Image
FROM builder AS buildenv

COPY . /src

RUN cmake -S /src \
  -B /build \
  -DCMAKE_BUILD_TYPE=Release && \
  cmake --build /build \
  --target all \
  --parallel $(nproc)

# Deployment Image
FROM scratch

COPY --from=buildenv /build/fabhub-web /fabhub-web

CMD ["/fabhub-web"]