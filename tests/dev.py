import sys
import zmq

toHex = lambda x:":".join([hex(ord(c))[2:].zfill(2) for c in x])

context = zmq.Context(1)

client = context.socket(zmq.DEALER)
client.connect("ipc:///tmp/rabbithole.ipc.28960")

msg = client.recv()
print('Received: %s' % (toHex(msg)))

client.close()