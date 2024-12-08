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
    return chi.ChicagoApp(
      title: 'Flutter Demo',
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
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

    return Row(
      children: [
        for (int i = 0; i < keyIcons.length; i++)
          IconButton(
            icon: Icon(keyIcons[i]),
            onPressed: () {
              repo.sendKey(id, keyCodes[i]);
            },
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
                trailing: IconButton(
                  icon: Icon(Icons.delete),
                  onPressed: () {
                    // TODO: delete (or not)
                  },
                ),
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
            children: [
              Text("Connection #$id"),
              VerticalDivider(),
              buildControls(context),
            ],
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
