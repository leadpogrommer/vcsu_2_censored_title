// import 'package:yaru/yaru.dart';
import 'package:chicago/chicago.dart' as chi;
import 'package:cnc_ui/repo.dart';
import 'package:flutter/material.dart';

void main() {
  repo.connectAndRun();
  runApp(const MyApp());
}

class ColoredText extends StatelessWidget {
  const ColoredText(this.text, this.color, {Key? key}) : super(key: key);

  final String text;
  final Color color;

  @override
  Widget build(BuildContext context) {
    final TextStyle baseStyle = DefaultTextStyle.of(context).style;
    final TextStyle coloredStyle = baseStyle.copyWith(color: color);
    return Text(text, style: coloredStyle);
  }
}

class WhiteText extends ColoredText {
  const WhiteText(String text, {Key? key})
      : super(text, const Color(0xffffffff), key: key);
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'С&C ui',
      home: const MyHomePage(title: 'С&C ui'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class DevicePage extends StatelessWidget {
  final int id;

  const DevicePage({super.key, required this.id});

  Widget buildControls(BuildContext context) {
    const keyCodes = [0, 2, 1];
    const keyIcons = [
      Icons.arrow_left,
      Icons.radio_button_checked_sharp,
      Icons.arrow_right
    ];

    final streams = repo.getDeviceStreams(id);
    const scale = 2.0;
    const w = 128 * scale;
    const h = 64 * scale;

    return Column(
      children: [
        StreamBuilder(
            stream: streams.image,
            builder: (context, snapshot) {
              final data = snapshot.data;

              if (data == null) {
                return const SizedBox(
                  width: w,
                  height: h,
                  child: Center(
                    child: Text('Image not availbale'),
                  ),
                );
              }

              return chi.BorderPane(
                child:
                    Image.memory(data, scale: 1 / scale, gaplessPlayback: true),
              );
            }),
        Row(
          children: [
            for (int i = 0; i < keyIcons.length; i++)
              IconButton(
                icon: Icon(keyIcons[i]),
                onPressed: () {
                  repo.sendKey(id, keyCodes[i]);
                },
              ),
          ],
        ),
      ],
    );
  }

  Widget buildProgDialog(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Run prog'),
      ),
      body: StreamBuilder(
        stream: repo.progListStream,
        builder: (ctx, snapshot) {
          var data = snapshot.data ?? [];
          return ListView.builder(
            itemBuilder: (context, index) {
              return ListTile(
                title: Text(data[index]),
                onTap: () {
                  repo.runProg(id, data[index]);
                  Navigator.of(context).pop();
                },
              );
            },
            itemCount: data.length,
          );
        },
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      floatingActionButton: FloatingActionButton(
        child: Text('Run prog'),
        onPressed: () => showDialog<void>(
          context: context,
          builder: (ctx) => Dialog.fullscreen(
            child: buildProgDialog(context),
          ),
        ),
      ),
      body: Column(
        children: [
          Row(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                mainAxisAlignment: MainAxisAlignment.start,
                children: [
                  Text(
                    "Connection #$id",
                    style: TextStyle(fontSize: 30),
                  ),
                  StreamBuilder(
                      stream: repo.getDeviceStreams(id).heapInfo,
                      builder: (context, snapshot) {
                        final data = snapshot.data ?? {};
                        final txt = data.entries
                            .map(
                                (e) => "${e.key}: ${(e.value / 1024).floor()}K")
                            .join("\n");
                        return Text(txt);
                      }),
                  // Spacer()
                ],
              ),
              Spacer(),
              buildControls(context),
            ],
          ),
          Divider(),
          Expanded(
            child: StreamBuilder(
              stream: repo.getDeviceStreams(id).taskInfo,
              builder: (context, snapshot) {
                final data = snapshot.data ?? [];
                return ListView.builder(
                  itemCount: data.length,
                  itemBuilder: (context, index) {
                    final task = data[index];
                    return ListTile(
                      title: Text("${task.id}: ${task.name}"),
                      onTap: ()=>repo.switchTask(id, task.id),
                      trailing: IconButton(
                        icon: Icon(Icons.delete),
                        onPressed: () => repo.endTask(id, task.id),
                      ),
                    );
                  },
                );
              }
            ),
          )
        ],
      ),
    );
  }
}

class _MyHomePageState extends State<MyHomePage> {
  // late TabController tabController;
  late chi.ListViewSelectionController _selectionController;
  int selectedConn = -1;

// late ScrollPaneController _paneController;

  @override
  void initState() {
    super.initState();
    // _selectionController = ListViewSelectionController();
    // _selectionController.
    // _selectionController.addListener((){
    //   setState(() {
    //     print('Setting index to $selectedIndex');
    //     selectedIndex = _selectionController.selectedIndex;
    //   });
    // });
    // _paneController.
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: StreamBuilder(
        stream: repo.deviceListStream,
        builder: (ctx, snapshot) {
          final data = snapshot.data ?? [];
          return Row(
            children: [
              Container(
                width: 60,
                alignment: Alignment.topLeft,
                child: Column(
                  children: [
                    const Text('Devices'),
                    const Divider(),
                    chi.ScrollableListView(
                      length: data
                          .length, // selectionController: _selectionController,
                      itemBuilder: (
                        ctx,
                        index,
                        selected,
                        highlighted,
                        disabled,
                      ) =>
                          chi.PushButton(
                        label: data[index].toString(),
                        onPressed: () => setState(() {
                          selectedConn = data[index];
                        }),
                      ),
                      itemHeight: 35,
                    ),
                  ],
                ),
              ),
              VerticalDivider(),
              Expanded(
                  child: data.contains(selectedConn)
                      ? DevicePage(id: selectedConn)
                      : Center(
                          child: Text('No connection selected'),
                        )),
            ],
          );
        },
      ),
    );
  }
}
