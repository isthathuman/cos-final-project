import socket
import requests
import threading
import argparse
import logging
import json
import sys

OPCODE_DATA = 1
OPCODE_WAIT = 2
OPCODE_DONE = 3
OPCODE_QUIT = 4
OPCODE_AGAIN = 5
OPCODE_CHANGE = 6


class Server:
    def __init__(self, name, algorithm, dimension, index, port, caddr, cport, ntrain, ntest):
        logging.info(
            "[*] Initializing the server module to receive data from the edge device")
        self.name = name
        self.algorithm = algorithm
        self.dimension = dimension
        self.index = index
        self.caddr = caddr
        self.cport = cport
        self.ntrain = ntrain
        self.ntest = ntest
        self.train_ts = set()  # train timestamp 집합 초기화
        self.test_ts = set()
        success = self.connecter()
        if success:
            self.port = port
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.bind(("0.0.0.0", port))
            self.socket.listen(10)
            self.listener()

    def connecter(self):
        success = True
        self.ai = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.ai.connect((self.caddr, self.cport))
        url = "http://{}:{}/{}".format(self.caddr, self.cport, self.name)
        request = {}
        request['algorithm'] = self.algorithm
        request['dimension'] = self.dimension
        request['index'] = self.index
        js = json.dumps(request)
        logging.debug("[*] To be sent to the AI module: {}".format(js))
        result = requests.post(url, json=js)
        response = json.loads(result.content)
        logging.debug("[*] Received: {}".format(response))

        if "opcode" not in response:
            logging.debug("[*] Invalid response")
            success = False
        else:
            if response["opcode"] == "failure":
                logging.error("Error happened")
                if "reason" in response:
                    logging.error("Reason: {}".format(response["reason"]))
                    logging.error("Please try again.")
                else:
                    logging.error("Reason: unknown. not specified")
                success = False
            else:
                assert response["opcode"] == "success"
                logging.info("[*] Successfully connected to the AI module")
        return success

    def listener(self):
        logging.info("[*] Server is listening on 0.0.0.0:{}".format(self.port))

        while True:
            client, info = self.socket.accept()
            logging.info(
                "[*] Server accept the connection from {}:{}".format(info[0], info[1]))

            client_handle = threading.Thread(
                target=self.handler, args=(client,))
            client_handle.start()

    def send_instance(self, vlst, is_training):

        logging.debug("puhaha")
        if is_training:
            url = "http://{}:{}/{}/training".format(
                self.caddr, self.cport, self.name)
        else:
            url = "http://{}:{}/{}/testing".format(
                self.caddr, self.cport, self.name)
        data = {}
        data["value"] = vlst
        req = json.dumps(data)
        logging.debug(req)
        response = requests.put(url, json=req)
        resp = response.json()

        if "opcode" in resp:
            if resp["opcode"] == "failure":
                logging.error("fail to send the instance to the ai module")

                if "reason" in resp:
                    logging.error(resp["reason"])
                else:
                    logging.error("unknown error")
                sys.exit(1)
        else:
            logging.error("unknown response")
            sys.exit(1)

    def parse_data(self, data, is_training, mode, client):
        json_data = json.loads(data.decode())
        min_temp = json_data["min_temp"]
        min_humid = json_data["min_humid"]
        max_temp = json_data["max_temp"]
        max_humid = json_data["max_humid"]
        power = json_data["power"]
        ts = json_data["timestamp"] - 1609459200
        month = json_data["month"]

        instance = [min_temp, min_humid, max_temp, max_humid, power, month]
        logging.info(
            "[min_temp, min_humid, max_temp, max_humid, power, month] = {}".format(instance))

        if is_training:
            if ts in self.train_ts and mode == 1:  # train 데이터의 timestamp 중복 체크
                logging.info(
                    "Duplicate timestamp found in training data. Requesting data again.")
                opcode = OPCODE_AGAIN
                client.send(int.to_bytes(opcode, 1, "big"))
                return False
            else:
                self.train_ts.add(ts)
                self.send_instance(instance, is_training)
                return True
        else:
            if ts in self.test_ts and mode == 1:   # test 데이터의 timestamp 중복 체크
                logging.info(
                    "Duplicate timestamp found in testing data. Requesting data again.")
                opcode = OPCODE_AGAIN
                client.send(int.to_bytes(opcode, 1, "big"))
                return False
            else:
                self.test_ts.add(ts)
                self.send_instance(instance, is_training)
                logging.info("")
                return True
# ------------------------------------------------------------------------------------------------------------------------

    # def parse_data(self, data, is_training, mode, client):
    #     json_data = json.loads(data.decode())
    #     average_temp = json_data["average_temp"]
    #     average_humid = json_data["average_humid"]
    #     power = json_data["power"]
    #     month = json_data["month"]
    #     ts = json_data["timestamp"] - 1609459200

    #     instance = [average_temp, average_humid, power, month]
    #     logging.info(
    #         "[average_temp, average_humid, power, month] = {}".format(instance))

    #     if is_training:
    #         if ts in self.train_ts and mode == 1:  # train 데이터의 timestamp 중복 체크
    #             logging.info(
    #                 "Duplicate timestamp found in training data. Requesting data again.")
    #             opcode = OPCODE_AGAIN
    #             client.send(int.to_bytes(opcode, 1, "big"))
    #             return False
    #         else:
    #             self.train_ts.add(ts)
    #             self.send_instance(instance, is_training)
    #             return True
    #     else:
    #         if ts in self.test_ts and mode == 1:   # test 데이터의 timestamp 중복 체크
    #             logging.info(
    #                 "Duplicate timestamp found in testing data. Requesting data again.")
    #             opcode = OPCODE_AGAIN
    #             client.send(int.to_bytes(opcode, 1, "big"))
    #             return False
    #         else:
    #             self.test_ts.add(ts)
    #             self.send_instance(instance, is_training)
    #             logging.info("")
    #             return True

    # TODO: You should implement your own protocol in this function
    # The following implementation is just a simple example

    def handler(self, client):
        logging.info("[*] Server starts to process the client's request")

        ntrain = self.ntrain
        url = "http://{}:{}/{}/training".format(
            self.caddr, self.cport, self.name)
        cnt = 0
        k = False
        mode = 0
        opcode = 0

        while True:
            # opcode (1 byte):
            cnt += 1
            print(cnt)
            rbuf = client.recv(1)
            opcode = int.from_bytes(rbuf, "big")
            logging.debug("[*] opcode: {}".format(opcode))
            rbuf = client.recv(1)
            mode = int.from_bytes(rbuf, "big")
            logging.debug("[*] mode: {}".format(mode))
            if mode == 3:
                break
            if opcode == OPCODE_DATA:
                logging.info("[*] data report from the edge")
                rbuf = client.recv(4096)  # 데이터 크기에 맞게 조정
                logging.debug("[*] received buf: {}".format(rbuf))
                k = self.parse_data(rbuf, True, mode, client)
            elif opcode == OPCODE_CHANGE:
                # logging.debug("[*] received the opcode OPCODE_CHANGE")
                # rbuf = client.recv(4096)  # 데이터 크기에 맞게 조정
                # logging.debug("[*] received buf: {}".format(rbuf))
                # k = self.parse_data(rbuf, True, mode, client)
                opcode = OPCODE_DONE
                client.send(int.to_bytes(opcode, 1, "big"))
                break
                ntrain = 0
            else:
                logging.error("[*] invalid opcode {}".format(opcode))
                logging.error("[*] please try again")
                sys.exit(1)

            if k:
                ntrain -= 1

            if ntrain > 0:
                opcode = OPCODE_DONE
                logging.debug("[*] send the opcode OPCODE_DONE")
                client.send(int.to_bytes(opcode, 1, "big"))
            else:
                opcode = OPCODE_WAIT
                logging.debug("[*] send the opcode OPCODE_WAIT")
                client.send(int.to_bytes(opcode, 1, "big"))
                break
        if mode != 3:
            logging.debug("Rquesting train")
            result = requests.post(url)
            response = json.loads(result.content)
            logging.debug(url)
            logging.debug("[*] return: {}".format(response["opcode"]))

            url = "http://{}:{}/{}/testing".format(
                self.caddr, self.cport, self.name)
            opcode = OPCODE_DONE
            logging.debug("[*] send the opcode OPCODE_DONE")
            client.send(int.to_bytes(opcode, 1, "big"))
            cnt2 = 0
            k = False
        ntest = self.ntest
        while ntest > 0:
            # opcode (1 byte):
            if mode == 3:
                break
            cnt2 += 1
            print(cnt2)
            rbuf = client.recv(1)
            opcode = int.from_bytes(rbuf, "big")
            logging.debug("[*] opcode: {}".format(opcode))
            rbuf = client.recv(1)
            mode = int.from_bytes(rbuf, "big")
            logging.debug("[*] mode: {}".format(mode))
            if mode == 3:
                break
            if opcode == OPCODE_DATA:
                # trash = client.recv(1)
                logging.info("[*] data report from the edge")
                rbuf = client.recv(4096)  # 데이터 크기에 맞게 조정
                logging.debug("[*] received buf: {}".format(rbuf))
                k = self.parse_data(rbuf, False, mode, client)
            elif opcode != OPCODE_CHANGE:
                logging.error("[*] invalid opcode")
                logging.error("[*] please try again")
                sys.exit(1)
            if k:
                ntest -= 1

            if ntest > 0:
                opcode = OPCODE_DONE
                client.send(int.to_bytes(opcode, 1, "big"))
            else:
                opcode = OPCODE_QUIT
                client.send(int.to_bytes(opcode, 1, "big"))
                break
        if mode == 3:
            opcode = OPCODE_QUIT
            client.send(int.to_bytes(opcode, 1, "big"))
        url = "http://{}:{}/{}/result".format(self.caddr,
                                              self.cport, self.name)
        result = requests.get(url)
        response = json.loads(result.content)
        logging.debug("response: {}".format(response))
        if "opcode" not in response:
            logging.error(
                "invalid response from the AI module: no opcode is specified")
            logging.error("please try again")
            sys.exit(1)
        else:
            if response["opcode"] == "failure":
                logging.error("getting the result from the AI module failed")
                if "reason" in response:
                    logging.error(response["reason"])
                logging.error("please try again")
                sys.exit(1)
            elif response["opcode"] == "success":
                self.print_result(response)
            else:
                logging.error("unknown error")
                logging.error("please try again")
                sys.exit(1)

    def print_result(self, result):
        logging.info("=== Result of Prediction ({}) ===".format(self.name))
        logging.info("   # of instances: {}".format(result["num"]))
        logging.debug("   sequence: {}".format(result["sequence"]))
        logging.debug("   prediction: {}".format(result["prediction"]))
        logging.info("   correct predictions: {}".format(result["correct"]))
        logging.info("   incorrect predictions: {}".format(
            result["incorrect"]))
        logging.info("   accuracy: {}\%".format(result["accuracy"]))


def command_line_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--algorithm", metavar="<AI algorithm to be used>",
                        help="AI algorithm to be used", type=str, required=True)
    parser.add_argument("-d", "--dimension", metavar="<Dimension of each instance>",
                        help="Dimension of each instance", type=int, default=1)
    parser.add_argument("-b", "--caddr", metavar="<AI module's IP address>",
                        help="AI module's IP address", type=str, required=True)
    parser.add_argument("-c", "--cport", metavar="<AI module's listening port>",
                        help="AI module's listening port", type=int, required=True)
    parser.add_argument("-p", "--lport", metavar="<server's listening port>",
                        help="Server's listening port", type=int, required=True)
    parser.add_argument("-n", "--name", metavar="<model name>",
                        help="Name of the model", type=str, default="model")
    parser.add_argument("-x", "--ntrain", metavar="<number of instances for training>",
                        help="Number of instances for training", type=int, default=10)
    parser.add_argument("-y", "--ntest", metavar="<number of instances for testing>",
                        help="Number of instances for testing", type=int, default=10)
    parser.add_argument("-z", "--index", metavar="<the index number for the power value>",
                        help="Index number for the power value", type=int, default=0)
    parser.add_argument("-l", "--log", metavar="<log level (DEBUG/INFO/WARNING/ERROR/CRITICAL)>",
                        help="Log level (DEBUG/INFO/WARNING/ERROR/CRITICAL)", type=str, default="INFO")
    args = parser.parse_args()
    return args


def main():
    args = command_line_args()
    logging.basicConfig(level=args.log)

    if args.ntrain <= 0 or args.ntest <= 0:
        logging.error(
            "Number of instances for training or testing should be larger than 0")
        sys.exit(1)

    Server(args.name, args.algorithm, args.dimension, args.index,
           args.lport, args.caddr, args.cport, args.ntrain, args.ntest)


if __name__ == "__main__":
    main()
