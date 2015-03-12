import sys
import zmq

context = zmq.Context(1)

client = context.socket(zmq.REQ)
client.connect("ipc:///var/cod4x17/server/broker.ipc")

client.send_string("ZMQ is love, ZMQ is life")

client.close()