import std.stdio : readln, writeln;

struct IO {
    import std.conv : to;
    import std.range : empty, front, popFront, split;

    string readToken() {
        while (tokens.empty) {
            tokens = readln.split;
        }
        auto token = tokens.front;
        tokens.popFront;
        return token;
    }

    int readInt() {
        return readToken.to!int;
    }

    string[] tokens;
}
