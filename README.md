# socket_programm
- block_socket 和 pthread_socket 案例
- mutli_client.py 使用gevent 测试模拟客户端连接情况和并发的新连接情况


测试的时候使用`ss  nc netstat`相关工具查看tcp状态需要关注
`TIME_WAIT CLOSE_WAIT FINIST_WAIT_2 `等状态是否正常

## TCP 连接建立与销毁 ##
- 3次握手建立连接
```
A                            B
|----SYN=1/seq=x        ---->|
|<---SYN=1/seq=y/ACK=x+1-----|
|----ACK=y+1            ---->|

```

- 4次挥手销毁连接（主动close方进入wait_time）
```
A                           B
|----FIN/seq=2x------------>|
finish_wait_1
|<---ACK=2x+1---------------| close_wait
finish_wait_2
|<---FIN/seq=2y-------------| last_ack
time_wait
|----ACK=2y+1-------------->| close
2mls
close
```

## pthread_create 线程操作 ##
- pthread_create 之后线程是否需要独立回收(pthread_detach)
- 主线程是否需要等待子线程结束(pthread_join)

大量创建和销毁子线程情况下是否有内存泄露（2MB/thread）

