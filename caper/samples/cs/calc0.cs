using System.Collections.Generic;

namespace calc
{
	enum Token
	{
		token_eof,
		token_Add,
		token_Div,
		token_Mul,
		token_Number,
		token_Sub,
	}

	interface ISemanticAction<Value>
	{
		void syntax_error();
		void stack_overflow();

		void upcast( out Value x, int y );
		void downcast( out int x, Value y );

		void Identity( out int arg0, int arg1);
		void MakeAdd( out int arg0, int arg1, int arg2);
		void MakeDiv( out int arg0, int arg1, int arg2);
		void MakeMul( out int arg0, int arg1, int arg2);
		void MakeSub( out int arg0, int arg1, int arg2);
	}

	class Parser<Value> where Value : class, new()
	{
		private class stack_frame
		{
			public state_type state;
			public gotof_type gotof;
			public Value value;

			public stack_frame(state_type s, gotof_type g, Value v)
			{
				state = s; gotof = g; value = v;
			}
		}

		private class Stack
		{
			private List<stack_frame> stack = new List<stack_frame>();
			private List<stack_frame> tmp = new List<stack_frame>();
			private int gap;

			public Stack(){ this.gap = 0; }
			public void reset_tmp()
			{
				this.gap = this.stack.Count;
				this.tmp.Clear();
			}

			public void commit_tmp()
			{
				int size = this.gap + this.tmp.Count;
				if(size > this.stack.Capacity) this.stack.Capacity = size;
				this.stack.RemoveRange(this.gap, this.stack.Count - this.gap);
				this.stack.AddRange(this.tmp);
			}
			public bool push(stack_frame f)
			{
				this.tmp.Add(f);
				return true;
			}

			public void pop(int n)
			{
				if(this.tmp.Count < n)
				{
				n -= this.tmp.Count;
				this.tmp.Clear();
				this.gap -= n;
				}else
				{
					this.tmp.RemoveRange(this.tmp.Count - n, this.tmp.Count);
				}
			}

			public stack_frame top()
			{
				if( this.tmp.Count != 0 )
				{
					return this.tmp[this.tmp.Count - 1];
				}else
				{
					return this.stack[this.gap - 1];
				}
			}

			public stack_frame get_arg(int b, int i)
			{
				int n = this.tmp.Count;
				if(b - i <= n)
				{
					return this.tmp[n - (b - i)];
				}else
				{
					return this.stack[this.gap - (b - n) + i];
				}
			}

			public void clear()
			{
				this.stack.Clear();
			}

		} // class Stack

		private delegate bool state_type(Token token, Value value);
		private delegate bool gotof_type(int i, Value value);

		public Parser(ISemanticAction<Value> sa)
		{
			this.stack = new Stack();
			this.sa = sa;
			this.reset();
		}


		public void reset()
		{
			this.error = false;
			this.accepted = false;
			this.clear_stack();
			this.reset_tmp_stack();
			if( this.push_stack( this.state_0, this.gotof_0, new Value()) )
			{
				this.commit_tmp_stack();
			}else
			{
				this.sa.stack_overflow();
				this.error = true;
			}
		}
		public bool post(Token token,Value value)
		{
			System.Diagnostics.Debug.Assert(!this.error);
			this.reset_tmp_stack();
			while(stack_top().state(token, value));
			if( !this.error )
			{
				this.commit_tmp_stack();
			}
			return this.accepted;
		}

		public bool accept(ref Value v)
		{
			System.Diagnostics.Debug.Assert(this.accepted);
			if(this.error) { return false; }
			v = this.accepted_value;
			return true;
		}

		public bool Error() { return this.error; }

		private ISemanticAction<Value> sa;
		private Stack stack;
		private bool accepted;
		private bool error;
		private Value accepted_value;

		private bool push_stack(state_type s, gotof_type g, Value v)
		{
			bool f = this.stack.push(new stack_frame(s, g, v));
			System.Diagnostics.Debug.Assert(!this.error);
			if(!f)
			{
				this.error = true;
				this.sa.stack_overflow();
			}
			return f;
		}

		private void pop_stack(int n)
		{
			this.stack.pop(n);
		}

		private stack_frame stack_top()
		{
			return this.stack.top();
		}

		private Value get_arg(int b, int i)
		{
			return stack.get_arg(b, i).value;
		}

		private void clear_stack()
		{
			this.stack.clear();
		}

		private void reset_tmp_stack()
		{
			this.stack.reset_tmp();
		}

		private void commit_tmp_stack()
		{
			this.stack.commit_tmp();
		}

		bool gotof_0(int nonterminal_index, Value v)
		{
			switch(nonterminal_index)
			{
				case 0: return push_stack( this.state_1, this.gotof_1, v );
				case 1: return push_stack( this.state_2, this.gotof_2, v );
				default: System.Diagnostics.Debug.Assert(false); return false;
			}
		}

		bool state_0(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_Number:
				// shift
				push_stack( this.state_7, this.gotof_7, value);
				return false;
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_1(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_1(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_eof:
				// accept
				// run_semantic_action();
				this.accepted = true;
				this.accepted_value  = get_arg( 1, 0 );
				return false;
				case Token.token_Add:
				// shift
				push_stack( this.state_3, this.gotof_3, value);
				return false;
				case Token.token_Sub:
				// shift
				push_stack( this.state_5, this.gotof_5, value);
				return false;
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_2(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_2(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_eof:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(0, v);
				}
				case Token.token_Add:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(0, v);
				}
				case Token.token_Div:
				// shift
				push_stack( this.state_10, this.gotof_10, value);
				return false;
				case Token.token_Mul:
				// shift
				push_stack( this.state_8, this.gotof_8, value);
				return false;
				case Token.token_Sub:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(0, v);
				}
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_3(int nonterminal_index, Value v)
		{
			switch(nonterminal_index)
			{
				case 1: return push_stack( this.state_4, this.gotof_4, v );
				default: System.Diagnostics.Debug.Assert(false); return false;
			}
		}

		bool state_3(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_Number:
				// shift
				push_stack( this.state_7, this.gotof_7, value);
				return false;
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_4(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_4(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_eof:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeAdd( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(0, v);
				}
				case Token.token_Add:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeAdd( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(0, v);
				}
				case Token.token_Div:
				// shift
				push_stack( this.state_10, this.gotof_10, value);
				return false;
				case Token.token_Mul:
				// shift
				push_stack( this.state_8, this.gotof_8, value);
				return false;
				case Token.token_Sub:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeAdd( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(0, v);
				}
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_5(int nonterminal_index, Value v)
		{
			switch(nonterminal_index)
			{
				case 1: return push_stack( this.state_6, this.gotof_6, v );
				default: System.Diagnostics.Debug.Assert(false); return false;
			}
		}

		bool state_5(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_Number:
				// shift
				push_stack( this.state_7, this.gotof_7, value);
				return false;
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_6(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_6(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_eof:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeSub( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(0, v);
				}
				case Token.token_Add:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeSub( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(0, v);
				}
				case Token.token_Div:
				// shift
				push_stack( this.state_10, this.gotof_10, value);
				return false;
				case Token.token_Mul:
				// shift
				push_stack( this.state_8, this.gotof_8, value);
				return false;
				case Token.token_Sub:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeSub( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(0, v);
				}
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_7(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_7(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_eof:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(1, v);
				}
				case Token.token_Add:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(1, v);
				}
				case Token.token_Div:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(1, v);
				}
				case Token.token_Mul:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(1, v);
				}
				case Token.token_Sub:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(1, 0) );
					int r; this.sa.Identity( out r, arg0);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 1);
					return stack_top().gotof(1, v);
				}
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_8(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_8(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_Number:
				// shift
				push_stack( this.state_9, this.gotof_9, value);
				return false;
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_9(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_9(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_eof:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeMul( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Add:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeMul( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Div:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeMul( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Mul:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeMul( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Sub:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeMul( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_10(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_10(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_Number:
				// shift
				push_stack( this.state_11, this.gotof_11, value);
				return false;
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

		bool gotof_11(int nonterminal_index, Value v)
		{
			System.Diagnostics.Debug.Assert(false);
			return true;
		}

		bool state_11(Token token, Value value)
		{
			switch(token)
			{
				case Token.token_eof:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeDiv( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Add:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeDiv( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Div:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeDiv( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Mul:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeDiv( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
				case Token.token_Sub:
				// reduce
				{
                int arg0; this.sa.downcast( out arg0, get_arg(3, 0) );
                int arg1; this.sa.downcast( out arg1, get_arg(3, 2) );
					int r; this.sa.MakeDiv( out r, arg0, arg1);
					Value v; this.sa.upcast( out v, r );
					pop_stack( 3);
					return stack_top().gotof(1, v);
				}
			default:
				this.sa.syntax_error();
				this.error = true;
				return false;
			}
		}

	} // class Parser

} // namespace calc