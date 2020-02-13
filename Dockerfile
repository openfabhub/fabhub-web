FROM scratch

ARG app_executable

ADD $app_executable /

CMD ["/mofprint"]