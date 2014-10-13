# Haxe compiler is very fast, so we needs no dependency management

all:
	../../caper -haxe hello0.cpg Hello0Parser.hx --debug
	haxe hello0.hxml
	../../caper -haxe hello1.cpg Hello1Parser.hx --debug
	haxe hello1.hxml
	../../caper -haxe hello2.cpg Hello2Parser.hx --debug
	haxe hello2.hxml
	../../caper -haxe calc0.cpg Calc0Parser.hx --debug
	haxe calc0.hxml
	../../caper -haxe calc1.cpg Calc1Parser.hx --debug
	haxe calc1.hxml
	../../caper -haxe recovery0.cpg Recovery0Parser.hx --debug
	haxe recovery0.hxml
	../../caper -haxe recovery1.cpg Recovery1Parser.hx --debug
	haxe recovery1.hxml
	../../caper -haxe optional.cpg OptionalParser.hx --debug
	haxe optional.hxml
	../../caper -haxe list0.cpg List0Parser.hx --debug
	haxe list0.hxml
	../../caper -haxe list1.cpg List1Parser.hx --debug
	haxe list1.hxml

clean:
	rm *.js Hello0Parser.hx Hello1Parser.hx
