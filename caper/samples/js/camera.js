// In this file, the word 'window' does not mean in the context of GUI but window/viewport concept

function ViewportTransformer() {
    this.window = makeRect(0, 0, 128, 128);;
    this.viewport = makeRect(0, 0, 128, 128);;

    this.draw = function(c, eraseFunction, drawFunction) {
        var window = this.window;
        var viewport = this.viewport;

        var canvas = c[0];
        var ctx = canvas.getContext('2d');
        ctx.save();
        eraseFunction(ctx);
        ctx.beginPath();
        ctx.rect(
            viewport.p0.x,
            viewport.p0.y,
            viewport.p1.x - viewport.p0.x,
            viewport.p1.y - viewport.p0.y);
        ctx.clip();
        ctx.setTransform(1, 0, 0, 1, 0, 0); // set identity matrix
        ctx.translate(viewport.p0.x, viewport.p0.y);
        ctx.scale(
            (viewport.p1.x - viewport.p0.x) / (window.p1.x - window.p0.x),
            (viewport.p1.y - viewport.p0.y) / (window.p1.y - window.p0.y));
        ctx.translate(-window.p0.x, -window.p0.y);
        drawFunction(ctx);
        ctx.restore();
    };

    this.setWindow = function(x0, y0, x1, y1) {
        this.window = makeRect(x0, y0, x1, y1);
    };

    this.setViewport = function(x0, y0, x1, y1) {
        this.viewport = makeRect(x0, y0, x1, y1);
    };

    this.getViewport = function() {
        return this.viewport;
    };

    this.getWindow = function() {
        return this.window;
    };

    function makeRect(x0, y0, x1, y1) {
        return {
            p0: makePoint(x0, y0),
            p1: makePoint(x1, y1)
        };
    };

    function makePoint(x, y) {
        return {x: x, y: y};
    }
}

function Camera() {
    this.scale = 1.0;
    this.focalPoint = makePoint(0,0);
    this.viewportTransformer = new ViewportTransformer();
    this.dragStartMouseCoord = null;
    this.dragStartFocalPoint = null;

    this.setScale = function(a_scale) {
        this.scale = a_scale;
    };
    
    this.setFocalPoint = function(x, y) {
        this.focalPoint = makePoint(x, y);
        this.adjustWindow();
    };

    this.mouseEventToCanvasPos = function(ev) {
        var canvas = $(ev.target);
        function pageToCanvas(p) {
            return makePoint(
                p.x - canvas.offset().left,
                p.y - canvas.offset().top);
        }

        return pageToCanvas(makePoint(ev.pageX, ev.pageY));
    };

    this.mouseEventToWorldPos = function(ev) {
        // 処理的にはやや冗長だが、わかりやすさのためにこのまま
        var canvas = $(ev.target);
        var viewport = this.viewportTransformer.getViewport();
        var window = this.viewportTransformer.getWindow();
        
        function pageToCanvas(p) {
            return makePoint(
                p.x - canvas.offset().left,
                p.y - canvas.offset().top);
        }

        function canvasToViewport(p) {
            return makePoint(
                p.x - viewport.p0.x,
                p.y - viewport.p0.y);
        }

        function viewportToWindow(p) {
            return makePoint(
                p.x *
                    (window.p1.x - window.p0.x) /
                    (viewport.p1.x - viewport.p0.x),
                p.y *
                    (window.p1.y - window.p0.y) /
                    (viewport.p1.y - viewport.p0.y));
        }

        function windowToWorldPos(p) {
            return makePoint(
                p.x + window.p0.x,
                p.y + window.p0.y);
        }

        return windowToWorldPos(
            viewportToWindow(
                canvasToViewport(
                    this.mouseEventToCanvasPos(ev))));
    };

    this.injectMouseOperation = function(canvas, constraint, draw) {
        var self = this;

        canvas.mousedown(function(ev) {
            self.dragStartMouseCoord = self.mouseEventToCanvasPos(ev);
            self.dragStartFocalPoint = self.focalPoint;
        });

        canvas.mousemove(function(ev) {
            if (self.dragStartMouseCoord) {
                var p = self.mouseEventToCanvasPos(ev);
                var diff = makePoint(
                    p.x - self.dragStartMouseCoord.x,
                    p.y - self.dragStartMouseCoord.y);
                var q = constraint(
                    makePoint(
                        self.dragStartFocalPoint.x - diff.x / self.scale,
                        self.dragStartFocalPoint.y - diff.y / self.scale));
                self.setFocalPoint(q.x, q.y);
                draw();
            }
        });

        canvas.mouseup(function(ev) {
            self.dragStartMouseCoord = null;
        });

        canvas.mousewheel(function(ev, delta, deltaX, deltaY) {
            if (0 < self.scale + delta * 0.125) {
                self.scale += delta * 0.125;
            }
            self.adjustWindow();
            draw();
        });
    };

    this.intersectWithWindow = function(x0, y0, x1, y1) {
        var window = this.viewportTransformer.getWindow();
        if (x1 < window.p0.x) { return false; }
        if (y1 < window.p0.y) { return false; }
        if (window.p1.x <= x0) { return false; }
        if (window.p1.y <= y0) { return false; }
        return true;
    };

    this.draw = function(canvas, eraseFunction, drawFunction) {
        this.viewportTransformer.draw(canvas, eraseFunction, drawFunction);
    };
    
    this.setViewport = function(x0, y0, x1, y1) {
        this.viewportTransformer.setViewport(x0, y0, x1, y1);
    };

    this.adjustWindow = function() {
        var viewport = this.viewportTransformer.getViewport();
        var x = this.focalPoint.x;
        var y = this.focalPoint.y;
        var w = (viewport.p1.x - viewport.p0.x) / this.scale; 
        var h = (viewport.p1.y - viewport.p0.y) / this.scale; 
        this.viewportTransformer.setWindow(x-w/2, y-h/2, x-w/2+w, y-h/2+h);
    };

    this.save = function() {
        return {
            scale: this.scale,
            focalPoint: this.focalPoint
        };
    };

    this.load = function(x) {
        if (x) {
            this.scale = x.scale;
            this.focalPoint = x.focalPoint;
            this.adjustWindow();
        }
    };

    function makePoint(x, y) {
        return {x: x, y: y};
    }

}
