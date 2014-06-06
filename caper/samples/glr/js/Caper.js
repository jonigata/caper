var Caper = (function() {

    Array.prototype.find = function(f) {
        var r = null;
        this.forEach(function(x) {
            if (f(x)) {
                r = x;
            }
        });
        return r;
    };

    var exports = {};

    function TerminalNode(t) {
        this.terminal = t;
    }
    TerminalNode.prototype.describe = function() {
        return Grammar.getTokenLabel(this.terminal);
    };
    function NonterminalNode(n) {
        this.nonterminal = n;
    }
    NonterminalNode.prototype.describe = function() {
        return Grammar.getNonterminalLabel(this.nonterminal);
    };

    var vertexIndex = 0;
    function StateVertex(s, age) {
        this.state = s;
        this.children = [];
        this.index = vertexIndex++;
        this.x = age;
        this.y = 0;
    }
    StateVertex.prototype.describe = function() {
        return this.state;
    };
    function NodeVertex(n, age) {
        this.node = n;
        this.children = [];
        this.index = vertexIndex++;
        this.x = age;
        this.y = 0;
    }
    NodeVertex.prototype.describe = function() {
        return this.node.describe();
    };

    var Grammar = null;
    exports.import = function(module) {
        Grammar = module;
    };

    function Parser(sa) {
        this.U = [new StateVertex(Grammar.firstState, 0)];
        this.accept = false;
        this.age = 0;
    }
    exports.Parser = Parser;

    Parser.prototype = {
        postFirst : function(token) {
            function PostContext(heads) {
                this.token = token;
                this.A = heads.concat();
                this.R = [];
                this.Q = {};
            }
            return new PostContext(this.U);
        },
        postNext : function(context) {
            if (context.A.length != 0) {
                this.actor(context);
                return true;
            } else if (context.R.length != 0) {
                this.reducer(context);
                return true;
            } else {
                this.shifter(context);
                return false;
            }
        },
        actor: function(context) {
            var v = context.A.shift();
            Grammar.Table[v.state].state[context.token].forEach(
                function(alpha) {
                    if (alpha == null) { return; }
                    switch(alpha.method) {
                    case 'accept':
                        this.accetpt = true;
                        break;
                    case 'shift':
                        if (context.Q[alpha.target] == null) {
                            context.Q[alpha.target] = [];
                        }
                        context.Q[alpha.target].push(v);
                        break;
                    case 'reduce':
                        v.children.forEach(function(x) {
                            context.R.push({v: v, x: x, p: alpha.production});
                        });
                        break;
                    }
                });
        },
        reducer: function(context) {
            var a = context.R.shift(); // v, x, p
            var N = a.p.left;
            var self = this;
            this.ancestors(a.x, a.p.right.length).forEach(function(w) {
                var s = Grammar.Table[w.state].gotof[N];
                var u = self.U.find(function(u) { return u.state == s; });
                if (u != null) {
                    if (u.children.find(
                        function(z) { return w in z.children; }) != null) {
                        // do nothing
                    } else {
                        var z = new NodeVertex(
                            new NonterminalNode(N), self.age - 1);
                        u.children.push(z);
                        z.children.push(w);
                        if (u in context.A) {
                            Grammar.Table[u.state].state[context.token].forEach(
                                function(q) {
                                    if (q.method == 'reduce') {
                                        context.R.push(
                                            {v: u, x: z, p: q.production});
                                    }
                                });
                        }
                    }
                } else {
                    var u = new StateVertex(s, self.age);
                    var z = new NodeVertex(
                        new NonterminalNode(N), self.age - 1);
                    u.children.push(z);
                    z.children.push(w);
                    context.A.push(u);
                    self.U.push(u);
                }
            });
        },
        shifter: function(context) {
            console.log('shift');
            var Un = [];
            var self = this;
            for (var state in context.Q) {
                var w = new StateVertex(state, self.age + 2);
                Un.push(w);
                context.Q[state].forEach(function(v) {
                    var x = new NodeVertex(
                        new TerminalNode(context.token), self.age + 1);
                    w.children.push(x);
                    x.children.push(v);
                });
            }
            this.U = Un;
            this.age += 2;
        },
        ancestors: function(v, len) {
            // v is NodeVertex
            len = len * 2 - 1;
            var R = [v];
            while (0 < len) {
                var S = [];
                R.forEach(function(r) {
                    S = S.concat(r.children);
                });
                R = S;
                len--;
            }
            return R;
        },
        draw: function(drawer, context) {
            // collect all living vertices
            var totalSet = {};
            var currSet = {};
            if (context == null) {
                for (var i = 0 ; i < this.U.length ; i++) {
                    var p = this.U[i];
                    currSet[p.index] = p;
                }
            } else {
                for (var i = 0 ; i < context.A.length ; i++) {
                    var p = context.A[i];
                    currSet[p.index] = p;
                }
                for (var i = 0 ; i < context.R.length ; i++) {
                    var p = context.R[i].v;
                    currSet[p.index] = p;
                }
                for (var state in context.Q) {
                    context.Q[state].forEach(function(p) {
                        currSet[p.index] = p;
                    });
                }
            }

            while (true) {
                var keys = Object.keys(currSet);
                var n = keys.length;
                if (n == 0) {
                    break;
                }
                var nextSet = {};
                for (var i = 0 ; i < n ; i++) {
                    var key = keys[i];
                    var p = currSet[key];
                    totalSet[key] = p;
                    for (var j = 0 ; j < p.children.length ; j++) {
                        var q = p.children[j];
                        if (q != null) {
                            nextSet[q.index] = q;
                        }
                    }
                }
                currSet = nextSet;
            }

            var keys = Object.keys(totalSet);
            //console.log(keys.join(', '));

            // Xで分類
            var columns = [];
            for (var i = 0 ; i < keys.length ; i++) {
                var key = keys[i];
                var p = totalSet[key];
                if (columns[p.x] == null) {
                    columns[p.x] = [];
                }
                columns[p.x].push(p);
            }

            // vertex描画
            var gap = 0;
            for (var i = 0 ; i < columns.length ; i++) {
                if (columns[i] == null) { gap++; continue; }
                columns[i].sort(function(a, b) { return a.index - b.index; });

                for (var k = 0 ; k < columns[i].length ; k++) {
                    var p = columns[i][k];
                    p.y = k;
                    var s = p.describe();
                    drawer.drawVertex(p.index, s, p.x - gap, p.y);

                    for (var j = 0 ; j < p.children.length ; j++) {
                        var q = p.children[j];
                        if (q != null) {
                            drawer.drawVertexEdge(p.index, p.x, p.y,
                                                  q.index, q.x, q.y);
                        }
                    }

                }
            }
        }
    };

    return exports;
})();

exports.Caper = Caper;
