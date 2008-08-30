// 2008/08/15 Naoyuki Hirayama

////////////////////////////////////////////////////////////////
// llvmint
llvm::Value* llvmint( int n )
{
	return llvm::ConstantInt::get( llvm::Type::Int32Ty, n );
}

#if 0

////////////////////////////////////////////////////////////////
// make_printf_declaration
static llvm::Function*
make_printf_declaration( llvm::Module* module )
{
	std::vector<const llvm::Type*> ft_printf_args;
	ft_printf_args.push_back(
		llvm::PointerType::getUnqual(llvm::IntegerType::get(8)));

	llvm::FunctionType* ft_printf =
		llvm::FunctionType::get( 
				llvm::IntegerType::get(32), ft_printf_args, true);

	llvm::Function* f_printf =
		llvm::cast<llvm::Function>(
			module->getOrInsertFunction("printf", ft_printf));

	return f_printf;
}

////////////////////////////////////////////////////////////////
// make_puti_definition
static llvm::Function*
make_puti_definition( llvm::Module* module )
{
	// signature
	std::vector< const llvm::Type* > types;
	types.push_back( llvm::Type::Int32Ty );

	llvm::FunctionType* ft_puti =
		llvm::FunctionType::get(
			llvm::Type::Int32Ty, types, /* not vararg */ false );

	// function
	llvm::Function* f_puti =
		llvm::Function::Create(
			ft_puti, llvm::Function::InternalLinkage,
			"puti",
			module );

	// basic block
	llvm::BasicBlock* f_puti_bb = llvm::BasicBlock::Create("", f_puti);

	// argument
	llvm::Argument* arg0 = f_puti->arg_begin();
	arg0->setName( "arg0" );

	// format string: constant array
	llvm::Constant* format0_array = llvm::ConstantArray::get("%d\n", true);

	// format string: global variable
	llvm::GlobalVariable* format0 = new llvm::GlobalVariable(
		format0_array->getType(),
		true,
		llvm::GlobalValue::InternalLinkage,
		format0_array,
		"format0",
		module );

	// format string: address
	std::vector< llvm::Value* > format0_args;
	format0_args.push_back( llvmint(0) );
	format0_args.push_back( llvmint(0) );
	llvm::Instruction* format0_address = 
		llvm::GetElementPtrInst::Create(
			format0, format0_args.begin(), format0_args.end(),
			"format0_address", f_puti_bb );

	// call
	std::vector< llvm::Value* > printf_args;
	printf_args.push_back(format0_address);
	printf_args.push_back(arg0);
	llvm::CallInst::Create(
		module->getFunction( "printf" ),
		printf_args.begin(), printf_args.end(), "", f_puti_bb );

	// return
	f_puti_bb->getInstList().push_back( llvm::ReturnInst::Create( arg0 ) );

	return f_puti;
}

////////////////////////////////////////////////////////////////
// make_main_definition
llvm::Function*
make_main_definition(
	llvm::Module*	module ,
	leaf::Node*		n,
	std::ostream&	os )	
{
	// signature
	llvm::FunctionType* ft_main =
		llvm::FunctionType::get(
			llvm::Type::Int32Ty,
			std::vector<const llvm::Type*>(),
			/* not vararg */ false );

	// function
	llvm::Function* f_main =
		llvm::Function::Create(
			ft_main, llvm::Function::ExternalLinkage,
			"main",
			module );

	// basic block
	llvm::BasicBlock* bb = llvm::BasicBlock::Create( "EntryBlock", f_main );

	// AST‚©‚ç•ÏŠ·
	leaf::Context context( module, f_main, bb );
	n->encode( context );
	context.bb->getInstList().push_back(
		llvm::ReturnInst::Create(
			llvm::ConstantInt::get( llvm::Type::Int32Ty, n ) ) );

	return f_main;
}

#else

////////////////////////////////////////////////////////////////
// make_puti_declaration
static llvm::Function*
make_puti_declaration( llvm::Module* module )
{
	// signature
	std::vector< const llvm::Type* > types;
	types.push_back( llvm::Type::Int32Ty );

	llvm::FunctionType* ft_puti =
		llvm::FunctionType::get(
			llvm::Type::Int32Ty, types, /* not vararg */ false );

	// function
	llvm::Function* f_puti =
		llvm::Function::Create(
			ft_puti, llvm::Function::ExternalLinkage,
			"puti",
			module );

	return f_puti;
}

#endif

