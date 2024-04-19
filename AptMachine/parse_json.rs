use json::object::Object;
use json::JsonValue;
use std::fs::File;
use std::io::prelude::*;
use std::path::PathBuf;

pub fn parse_json_file(json_path: &PathBuf) -> Result<Object, String> {
    let mut file = File::open(json_path).map_err(|e| e.to_string())?;

    let mut json_str = String::new();
    let _ = file.read_to_string(&mut json_str).map_err(|e| e.to_string())?;

    let json_value = json::parse(&json_str).map_err(|e| e.to_string())?;

    into_json_obj(json_value)
}


pub fn get_json_value<'a>(json_obj: &'a Object, param: &str) -> Result<&'a JsonValue, String> {
    json_obj.get(param).ok_or(format!("missing param \'{}\'", param))
}



pub fn get_param_str(json_obj: &Object, param: &str) -> Result<String, String> {
    get_str(get_json_value(json_obj, param)?)
}



pub fn get_param_u64(json_obj: &Object, param: &str) -> Result<u64, String> {
    get_u64(get_json_value(json_obj, param)?)
}



pub fn get_param_f64(json_obj: &Object, param: &str) -> Result<f64, String> {
    get_f64(get_json_value(json_obj, param)?)
}



pub fn get_param_bool(json_obj: &Object, param: &str) -> Result<bool, String> {
    get_bool(get_json_value(json_obj, param)?)
}



pub fn get_param_array_of_u64(json_obj: &Object, param: &str) -> Result<Vec<u64>, String> {
    let array = get_param_array(json_obj, param)?;
    let mut vec = Vec::new();
    for v in array {
        vec.push(get_u64(v)?)
    }
    Ok(vec)
}



pub fn get_param_array<'a>(json_obj: &'a Object, param: &str) -> Result<&'a Vec<JsonValue>, String> {
    get_array(get_json_value(json_obj, param)?)
}



pub fn get_json_obj(value: &JsonValue) -> Result<&Object, String> {
    match value {
        JsonValue::Object(obj) => Ok(obj),
        _ => Err("json value is not object".to_string()),
    }
}



pub fn into_json_obj(value: JsonValue) -> Result<Object, String> {
    match value {
        JsonValue::Object(obj) => Ok(obj),
        _ => Err("json value is not object".to_string()),
    }
}



pub fn get_str(value: &JsonValue) -> Result<String, String> {
    if let Some(s) = value.as_str() {
        Ok(String::from(s))
    } else {
        Err(format!("param {} is not string", value))
    }
}



pub fn get_f64(value: &JsonValue) -> Result<f64, String> {
    match value {
        json::JsonValue::Number(n) => Ok((*n).into()),
        _ => Err(format!("param {} is not a number", value))
    }
}



pub fn get_u64(value: &JsonValue) -> Result<u64, String> {
    match value {
        json::JsonValue::Number(n) => Ok((*n).as_fixed_point_u64(0).ok_or("value is not u64")?),
        _ => Err(format!("param {} is not a number", value))
    }
}



pub fn get_bool(value: &JsonValue) -> Result<bool, String> {
    match value {
        json::JsonValue::Boolean(b) => Ok(*b),
        _ => Err(format!("param {} is not a number", value))
    }
}



pub fn get_array(value: &JsonValue) -> Result<&Vec<JsonValue>, String> {
    match value {
        json::JsonValue::Array(vec) => Ok(vec),
        _ => Err(format!("param {} is not an array", value))
    }
}
