import 'dart:convert';

import 'package:rxdart/rxdart.dart';
import 'package:web_socket_channel/web_socket_channel.dart';



class Repo{
  late final WebSocketChannel ws;

  final _deviceListController = BehaviorSubject<List<int>>();
  late final ValueStream<List<int>> deviceListStream;

  final _progListController = BehaviorSubject<List<String>>();
  late final ValueStream<List<String>> progListStream;

  Repo(){
    deviceListStream = _deviceListController.stream;
    progListStream = _progListController.stream;
  }

  void connectAndRun() async {
    ws = WebSocketChannel.connect(Uri.parse("ws://127.0.0.1:8080/front"));
    await ws.ready;
    ws.stream.listen((msg){
      print('Got message:  $msg');
      final req = jsonDecode(msg);
      final what = req['what'];
      if(what == 'devices'){
        _deviceListController.sink.add((req['data'] as List<dynamic>).cast<int>());
      } else if(what == 'progs') {
        _progListController.sink.add(req['data'].cast<String>());
      }else if(what == 'tasks'){
        print('Got tasks for ${req["did"]}: ${req["data"]}');
      }else{
        print('Unknown message: $what');
      }
    });
  }

  void sendKey(int did, int key) async {
    ws.sink.add(json.encode({'what': 'key', 'did': did, 'data': key}));
  }
  
  void runProg(int did, String name) {
    ws.sink.add(json.encode({'what': 'run', 'did': did, 'data': name}));
  }
}

final repo = Repo();