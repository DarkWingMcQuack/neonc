struct TextPosition{
	column: u64,
	row: u64
}

pub(crate) struct TextArea <'a>{
	filename: &'a str,
	content: &'a str,
	start: TextPosition,
	end: TextPosition
}
