var Caper = require('./Caper.js').Caper;
var Foo = require('./Foo.js').Foo;
Caper.import(Foo);
console.log(Foo.Token.token_det);

function Iterator(root) {
    var self = this;
    function TNode(name) {
        this.name = name;
        this.show = function() {
            return '\'' + this.name + '\'';
        };
    }
    function NNode(name) {
        this.name = name; this.children = [];
        this.show = function() {
            var s = '(' + this.name + ':';
            for (var i = 0 ; i < this.children.length ; i++) {
                s += ' ' + this.children[i].show();
            }
            return s + ')';
        };
    }

    function build(node, cursor) {
        if (node.nodeType == 'TNode') {
            return new TNode(node.value);
        }
        
        var r = new NNode(Foo.getNonterminalLabel(node.nonterminal));
        var choise = 0;
        if (1 < node.successors.length) {
            if (cursor < self.stacktrace.length) {
                var frame = self.stacktrace[cursor];
                if (frame.source !== node) {throw 'bad stacktrace';}
                choise = frame.choise;
            } else {
                self.stacktrace.push({ source: node, choise: 0 });
            }
            cursor++;
        }
        var s = node.successors[choise];
        for (var i = 0 ; i < s.length ; i++) {
            r.children.push(build(s[i], cursor));
        }
        return r;
    }

    function iterate() {
        while(0 < self.stacktrace.length) {
            var x = self.stacktrace[self.stacktrace.length - 1];
            var c = x.choise + 1;
            if (c < x.source.successors.length) {
                x.choise = c;
                return true;
            }
            self.stacktrace.pop();
        }
        return false;
    }

    this.next = function() {
        if (this.stacktrace == null) {
            this.stacktrace = [];
        } else {
            if (!iterate()) {
                return null;
            }
        }
        return build(root, 0);
    };

    this.stacktrace = null;
}


function parse() {
    var data = [
        [Foo.Token.token_n, "I"],
        [Foo.Token.token_v, "saw"],
        [Foo.Token.token_det, "a"],
        [Foo.Token.token_n, "man"],
        [Foo.Token.token_prep, "in"],
        [Foo.Token.token_det, "the"],
        [Foo.Token.token_n, "park"],
        [Foo.Token.token_prep, "with"],
        [Foo.Token.token_det, "a"],
        [Foo.Token.token_n, "scope"],
        [Foo.Token.token_eof, "$"]
    ];
    var parser = new Caper.Parser(null);

    var context = null;
    function step() {
        if (context == null) {
            if (data.length == 0) {
                return false;
            }
            var d = data.shift();
            context = parser.postFirst(d[0], d[1]);
            console.log('postFirst');
            return true;
        }
        console.log('postNext');
        if(!parser.postNext(context)) {
            context = null;
        }
        return true;
    };

    while(step()) {
        var drawer = {};
        parser.draw(drawer, context);
    }

/*
    var it = new Iterator(parser.accepts[0].node());
    console.log(it.next().show());
    console.log(it.next().show());
    console.log(it.next().show());
    while(true) {
        var x = it.next();
        if (x == null) {
            break;
        }
        console.log(x.show());
    }
*/
}

parse();
