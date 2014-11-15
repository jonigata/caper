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
	
	haxe -main Hello0 -neko Hello0.n
	neko Hello0.n
	haxe -main Hello1 -neko Hello1.n
	neko Hello1.n
	haxe -main Calc0 -neko Calc0.n
	neko Calc0.n
	haxe -main Calc1 -neko Calc1.n
	neko Calc1.n
	haxe -main Recovery0 -neko Recovery0.n
	neko Recovery0.n
	haxe -main Recovery1 -neko Recovery1.n
	neko Recovery1.n
	haxe -main List0 -neko List0.n
	neko List0.n
	haxe -main List1 -neko List1.n
	neko List1.n
	haxe -main Optional -neko Optional.n
	neko Optional.n

clean:
	rm *.js Hello0Parser.hx Hello1Parser.hx
