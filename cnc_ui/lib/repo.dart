import 'dart:convert';
import 'dart:typed_data';

import 'package:rxdart/rxdart.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class TaskInfo{
  final int id;
  final String name;

  TaskInfo({required this.id, required this.name});
}

class DeviceStreams{
  final BehaviorSubject<Map<String, int>> _heapInfo = BehaviorSubject();
  final BehaviorSubject<List<TaskInfo>> _taskInfo = BehaviorSubject();
  final BehaviorSubject<Uint8List> _image = BehaviorSubject();

  ValueStream<Map<String, int>> get heapInfo => _heapInfo.stream;
  ValueStream<List<TaskInfo>> get taskInfo => _taskInfo.stream;
  ValueStream<Uint8List> get image => _image.stream;
}


class Repo{
  late final WebSocketChannel ws;

  final _deviceListController = BehaviorSubject<List<int>>();
  late final ValueStream<List<int>> deviceListStream;

  final _progListController = BehaviorSubject<List<String>>();
  late final ValueStream<List<String>> progListStream;

  final Map<int, DeviceStreams> _deviceStreamsMap = {};

  Repo(){
    deviceListStream = _deviceListController.stream;
    progListStream = _progListController.stream;
  }

  DeviceStreams getDeviceStreams(int id){
    if(_deviceStreamsMap.containsKey(id)){
      return _deviceStreamsMap[id]!;
    }
    print('WARNING: trying to get device streams for non-existent device');
    return DeviceStreams(); // return dummy streams
  }

  void connectAndRun() async {
    ws = WebSocketChannel.connect(Uri.parse("ws://127.0.0.1:8080/front"));
    await ws.ready;
    ws.stream.listen((msg){
      // print('Got message:  $msg');
      final req = jsonDecode(msg) as Map<String, dynamic>;
      final what = req['what'] as String;
      final data = req['data'];
      final did = req['did'] as int?;

      switch(what){
        case 'devices':
          final devs = (req['data'] as List<dynamic>).cast<int>();
          // Add new devices
          for(final dev in devs){
            if (!_deviceStreamsMap.containsKey(dev)){
              _deviceStreamsMap[dev] = DeviceStreams();
            }
          }
          // remove old devices
          for(final dev in _deviceStreamsMap.keys){
            if(!devs.contains(dev)){
              _deviceStreamsMap.remove(dev);
            }
          }
          _deviceListController.sink.add(devs);
        case 'progs':
          _progListController.sink.add(req['data'].cast<String>());
        case 'tasks':
          _deviceStreamsMap[did!]?._taskInfo.sink.add((data as List).map((m) => TaskInfo(id: m['tid'], name: m['name'])).toList());
        case 'heap':
          _deviceStreamsMap[did!]?._heapInfo.sink.add((data as Map).cast());
        case 'img':
          _deviceStreamsMap[did!]?._image.sink.add(base64Decode(data));
        default:
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

  void switchTask(int did, int tid) {
    ws.sink.add(json.encode({'what': 'switch', 'did': did, 'data': tid}));
  }

  void endTask(int did, int tid) {
    ws.sink.add(json.encode({'what': 'end', 'did': did, 'data': tid}));
  }
}

final repo = Repo();