ARG CERVER_VERSION=2.0b-51
ARG HIREDIS_VERSION=1.0.2
ARG CREDIS_VERSION=0.5.2

ARG BUILD_DEPS='ca-certificates git libssl-dev libcurl4-openssl-dev'
ARG RUNTIME_DEPS='ca-certificates libssl-dev libcurl4-openssl-dev'

FROM ermiry/mongoc:builder as builder

WORKDIR /opt

ARG BUILD_DEPS
RUN apt-get --allow-releaseinfo-change update && apt-get install -y ${BUILD_DEPS}

# hiredis
ARG HIREDIS_VERSION
RUN mkdir /opt/hiredis && cd /opt/hiredis \
    && wget -O hiredis.tar.gz https://github.com/redis/hiredis/archive/v${HIREDIS_VERSION}.tar.gz \
    && tar xzf hiredis.tar.gz \
    && cd /opt/hiredis/hiredis-${HIREDIS_VERSION} \
    && make -j4 && make install

# build cerver with production flags
ARG CERVER_VERSION
RUN mkdir /opt/cerver && cd /opt/cerver \
    && wget -q --no-check-certificate https://github.com/ermiry/cerver/archive/${CERVER_VERSION}.zip \
    && unzip ${CERVER_VERSION}.zip \
    && cd cerver-${CERVER_VERSION} \
    && make TYPE=production -j4 && make TYPE=production install

# credis
ARG CREDIS_VERSION
RUN mkdir /opt/credis && cd /opt/credis \
    && wget -q --no-check-certificate https://github.com/ermiry-com/credis/archive/${CREDIS_VERSION}.zip \
    && unzip ${CREDIS_VERSION}.zip \
    && cd credis-${CREDIS_VERSION} \
    && make TYPE=production -j4 && make TYPE=production install

RUN ldconfig

# watch
WORKDIR /opt/watch
COPY . .
RUN make TYPE=production -j4

############
FROM ermiry/mongoc:latest

ARG RUNTIME_DEPS
RUN apt-get update && apt-get install -y ${RUNTIME_DEPS}

# hiredis
ARG HIREDIS_VERSION
COPY --from=builder /opt/hiredis/hiredis-${HIREDIS_VERSION}/libhiredis.so /usr/local/lib/
COPY --from=builder /usr/local/include/hiredis /usr/local/include/hiredis

# cerver
ARG CERVER_VERSION
COPY --from=builder /opt/cerver/cerver-${CERVER_VERSION}/bin/libcerver.so /usr/local/lib/
COPY --from=builder /opt/cerver/cerver-${CERVER_VERSION}/include/cerver /usr/local/include/cerver

# credis
ARG CREDIS_VERSION
COPY --from=builder /opt/credis/credis-${CREDIS_VERSION}/bin/libcredis.so /usr/local/lib/
COPY --from=builder /opt/credis/credis-${CREDIS_VERSION}/include/credis /usr/local/include/credis

RUN ldconfig

# watch
WORKDIR /home/watch
COPY ./start.sh .
COPY --from=builder /opt/watch/bin ./bin

CMD ["/bin/bash", "start.sh"]
