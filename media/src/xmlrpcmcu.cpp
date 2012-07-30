#include <string>
#include <vector>

#include "xmlhandler.h"
#include "mcu.h"

//CreateConference
xmlrpc_value* CreateConference(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;
	UTF8Parser tagParser;
	 //Parseamos
	char *str;
	int queueId = 0;
	xmlrpc_parse_value(env, param_array, "(si)", &str, &queueId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
	{
		//Try with old parameters
		xmlrpc_parse_value(env, param_array, "(s)", &str);

		//Comprobamos si ha habido error
		if(env->fault_occurred)
			//Send errro
			xmlerror(env,"Fault occurred\n");
	}

	//Parse string
	tagParser.Parse((BYTE*)str,strlen(str));

	//Creamos la conferencia
	int confId = mcu->CreateConference(tagParser.GetWString(),queueId);

	//Si error
	if (!confId>0)
		return xmlerror(env,"No se puede crear la conferencia");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conferencia borrada antes de poder iniciarse\n");

	//La iniciamos
	int res = conf->Init();

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No se ha podido iniciar la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",confId));
}

xmlrpc_value* DeleteConference(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	xmlrpc_parse_value(env, param_array, "(i)", &confId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
			xmlerror(env,"Fault occurred\n");

	//Delete conference 
	if (!mcu->DeleteConference(confId))
		return xmlerror(env,"Conference does not exist");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* GetConferences(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MCU::ConferencesInfo list;

	//Obtenemos la referencia
	if(!mcu->GetConferenceList(list))
		return xmlerror(env,"Could not retreive conference info list\n");

	//Create array
	xmlrpc_value* arr = xmlrpc_array_new(env);

	//Process result
	for (MCU::ConferencesInfo::iterator it = list.begin(); it!=list.end(); ++it)
	{
		//Create array
		xmlrpc_value* val = xmlrpc_build_value(env,"(isi)",it->second.id,it->second.name.c_str(),it->second.numPart);
		//Add it
		xmlrpc_array_append_item(env,arr,val);
		//Release
		xmlrpc_DECREF(val);
	}

	//return
	return xmlok(env,arr);
}

xmlrpc_value* CreateMosaic(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int comp;
	int size;
	xmlrpc_parse_value(env, param_array, "(iii)", &confId,&comp,&size);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int mosaicId = conf->CreateMosaic((Mosaic::Type)comp,size);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!mosaicId)
		return xmlerror(env,"Could not create mosaic\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",mosaicId));
}

xmlrpc_value* SetMosaicOverlayImage(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int mosaicId;
	char *filename;
	xmlrpc_parse_value(env, param_array, "(iis)", &confId,&mosaicId,&filename);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SetMosaicOverlayImage(mosaicId,filename);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could set overlay image\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",mosaicId));
}

xmlrpc_value* ResetMosaicOverlay(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int mosaicId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&mosaicId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->ResetMosaicOverlay(mosaicId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could reset overlay image\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",mosaicId));
}
xmlrpc_value* DeleteMosaic(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int mosaicId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&mosaicId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->DeleteMosaic(mosaicId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could reset overlay image\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",mosaicId));
}

xmlrpc_value* CreateParticipant(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;
	UTF8Parser parser;

	 //Parseamos
	int confId;
	int mosaicId;
	char *name;
	int type;
	xmlrpc_parse_value(env, param_array, "(iisi)", &confId,&mosaicId,&name,&type);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//Parse string
	parser.Parse((BYTE*)name,strlen(name));

	//La borramos
	int partId = conf->CreateParticipant(mosaicId,parser.GetWString(),(Participant::Type)type);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!partId)
		return xmlerror(env,"No se ha podido a�adir el participante a la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",partId));
}

xmlrpc_value* DeleteParticipant(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->DeleteParticipant(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No se ha podido a�adir el participante a la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env);
}
xmlrpc_value* AddConferenceToken(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(is)", &confId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	bool res  = conf->AddBroadcastToken(parser.GetWString());

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not add pin to broadcast session\n");

	//Devolvemos el resultado
	return xmlok(env);
}
xmlrpc_value* AddParticipantInputToken(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(iis)", &confId, &partId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	bool res  = conf->AddParticipantInputToken(partId,parser.GetWString());

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not add pin to participant input\n");

	//Devolvemos el resultado
	return xmlok(env);
}
xmlrpc_value* AddParticipantOutputToken(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(iis)", &confId, &partId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	bool res  = conf->AddParticipantOutputToken(partId,parser.GetWString());

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not add pin to participant output\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartBroadcaster(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	xmlrpc_parse_value(env, param_array, "(i)", &confId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int port = conf->StartBroadcaster();

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!port)
		return xmlerror(env,"No se ha podido crear el watcher FLV\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",port));
}

xmlrpc_value* StopBroadcaster(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	xmlrpc_parse_value(env, param_array, "(i)", &confId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopBroadcaster();
	
	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No se ha podido a�adir el participante a la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* SetVideoCodec(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	int codec;
	int mode;
	int fps;
	int bitrate;
	int quality;
	int fillLevel;
	int intraPeriod;

	xmlrpc_parse_value(env, param_array, "(iiiiiiiii)", &confId,&partId,&codec,&mode,&fps,&bitrate,&quality,&fillLevel,&intraPeriod);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SetVideoCodec(partId,codec,mode,fps,bitrate,quality,fillLevel,intraPeriod);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No soportado\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartSendingVideo(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	char *sendVideoIp;
	int sendVideoPort;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iisiS)", &confId,&partId,&sendVideoIp,&sendVideoPort,&rtpMap);

	//Get the rtp map
	VideoCodec::RTPMap map;

	//Get map size
	int j = xmlrpc_struct_size(env,rtpMap);

	//Parse rtp map
	for (int i=0;i<j;i++)
	{
		xmlrpc_value *key, *val;
		const char *type;
		int codec;
		//Read member
		xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
		//Read name
		xmlrpc_parse_value(env,key,"s",&type);
		//Read value
		xmlrpc_parse_value(env,val,"i",&codec);
		//Add to map
		map[atoi(type)] = (VideoCodec::Type) codec;
		//Decrement ref counter
		xmlrpc_DECREF(key);
		xmlrpc_DECREF(val);
	}

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StartSendingVideo(partId,sendVideoIp,sendVideoPort,map) != NULL;

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StopSendingVideo(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopSendingVideo(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No se ha podido terminar la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartReceivingVideo(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iiS)", &confId,&partId,&rtpMap);

	//Get the rtp map
	VideoCodec::RTPMap map;

	//Get map size
	int j = xmlrpc_struct_size(env,rtpMap);

	//Parse rtp map
	for (int i=0;i<j;i++)
	{
		xmlrpc_value *key, *val;
		const char *type;
		int codec;
		//Read member
		xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
		//Read name
		xmlrpc_parse_value(env,key,"s",&type);
		//Read value
		xmlrpc_parse_value(env,val,"i",&codec);
		//Add to map
		map[atoi(type)] = (VideoCodec::Type) codec;
		//Decrement ref counter
		xmlrpc_DECREF(key);
		xmlrpc_DECREF(val);
	}

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int recVideoPort = conf->StartReceivingVideo(partId,map);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!recVideoPort)
		return xmlerror(env,"No se ha podido terminar la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",recVideoPort));
}

xmlrpc_value* StopReceivingVideo(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopReceivingVideo(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* SetAudioCodec(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	int codec;

	xmlrpc_parse_value(env, param_array, "(iii)", &confId,&partId,&codec);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SetAudioCodec(partId,codec);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No soportado\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartSendingAudio(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	char *sendAudioIp;
	int sendAudioPort;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iisiS)", &confId,&partId,&sendAudioIp,&sendAudioPort,&rtpMap);

	//Get the rtp map
	AudioCodec::RTPMap map;

	int j = xmlrpc_struct_size(env,rtpMap);

	//Parse rtp map
	for (int i=0;i<j;i++)
	{
		xmlrpc_value *key, *val;
		const char *type;
		int codec;
		//Read member
		xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
		//Read name
		xmlrpc_parse_value(env,key,"s",&type);
		//Read value
		xmlrpc_parse_value(env,val,"i",&codec);
		//Add to map
		map[atoi(type)] = (AudioCodec::Type) codec;
		//Decrement ref counter
		xmlrpc_DECREF(key);
		xmlrpc_DECREF(val);
	}

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StartSendingAudio(partId,sendAudioIp,sendAudioPort,map) != NULL;

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StopSendingAudio(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopSendingAudio(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No se ha podido terminar la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartReceivingAudio(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iiS)", &confId,&partId,&rtpMap);

	//Get the rtp map
	AudioCodec::RTPMap map;

	int j = xmlrpc_struct_size(env,rtpMap);

	//Parse rtp map
	for (int i=0;i<j;i++)
	{
		xmlrpc_value *key, *val;
		const char *type;
		int codec;
		//Read member
		xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
		//Read name
		xmlrpc_parse_value(env,key,"s",&type);
		//Read value
		xmlrpc_parse_value(env,val,"i",&codec);
		//Add to map
		map[atoi(type)] = (AudioCodec::Type) codec;
		//Decrement ref counter
		xmlrpc_DECREF(key);
		xmlrpc_DECREF(val);
	}

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int recAudioPort = conf->StartReceivingAudio(partId,map);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!recAudioPort)
		return xmlerror(env,"No se ha podido terminar la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",recAudioPort,recAudioPort));
}

xmlrpc_value* StopReceivingAudio(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopReceivingAudio(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* SetTextCodec(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	int codec;

	xmlrpc_parse_value(env, param_array, "(iii)", &confId,&partId,&codec);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SetTextCodec(partId,codec);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No soportado\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartSendingText(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	char *sendTextIp;
	int sendTextPort;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iisiS)", &confId,&partId,&sendTextIp,&sendTextPort,&rtpMap);

	//Get the rtp map
	TextCodec::RTPMap map;

	int j = xmlrpc_struct_size(env,rtpMap);

	//Parse rtp map
	for (int i=0;i<j;i++)
	{
		xmlrpc_value *key, *val;
		const char *type;
		int codec;
		//Read member
		xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
		//Read name
		xmlrpc_parse_value(env,key,"s",&type);
		//Read value
		xmlrpc_parse_value(env,val,"i",&codec);
		//Add to map
		map[atoi(type)] = (TextCodec::Type) codec;
		//Decrement ref counter
		xmlrpc_DECREF(key);
		xmlrpc_DECREF(val);
	}

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StartSendingText(partId,sendTextIp,sendTextPort,map);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StopSendingText(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopSendingText(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"No se ha podido terminar la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartReceivingText(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iiS)", &confId,&partId,&rtpMap);

	//Get the rtp map
	TextCodec::RTPMap map;

	int j = xmlrpc_struct_size(env,rtpMap);

	//Parse rtp map
	for (int i=0;i<j;i++)
	{
		xmlrpc_value *key, *val;
		const char *type;
		int codec;
		//Read member
		xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
		//Read name
		xmlrpc_parse_value(env,key,"s",&type);
		//Read value
		xmlrpc_parse_value(env,val,"i",&codec);
		//Add to map
		map[atoi(type)] = (TextCodec::Type) codec;
		//Decrement ref counter
		xmlrpc_DECREF(key);
		xmlrpc_DECREF(val);
	}

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int recTextPort = conf->StartReceivingText(partId,map);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!recTextPort)
		return xmlerror(env,"No se ha podido terminar la conferencia\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",recTextPort,recTextPort));
}

xmlrpc_value* StopReceivingText(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopReceivingText(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* SetCompositionType(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int mosaicId;
	int comp;
	int size;
	xmlrpc_parse_value(env, param_array, "(iiii)", &confId,&mosaicId,&comp,&size);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SetCompositionType(mosaicId,(Mosaic::Type)comp,size);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* SetParticipantMosaic(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	int mosaicId;
	xmlrpc_parse_value(env, param_array, "(iii)", &confId,&partId,&mosaicId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SetParticipantMosaic(partId,mosaicId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}


xmlrpc_value* SetMosaicSlot(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int mosaicId;
	int num;
	int id;
	xmlrpc_parse_value(env, param_array, "(iiii)", &confId,&mosaicId,&num,&id);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");
	 
	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//Set slot
	int res = conf->SetMosaicSlot(mosaicId,num,id);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* AddMosaicParticipant(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int mosaicId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(iii)", &confId,&mosaicId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//Set slot
	int res = conf->AddMosaicParticipant(mosaicId,partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RemoveMosaicParticipant(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int mosaicId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(iii)", &confId,&mosaicId,&partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//Set slot
	int res = conf->RemoveMosaicParticipant(mosaicId,partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* CreatePlayer(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;
	UTF8Parser parser;
	
	 //Parseamos
	int confId;
	int privateId;
	char *name;
	xmlrpc_parse_value(env, param_array, "(iis)", &confId,&privateId,&name);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//Parse string
	parser.Parse((BYTE*)name,strlen(name));

	//La borramos
	int playerId = conf->CreatePlayer(privateId,parser.GetWString());

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!playerId)
		return xmlerror(env,"Could not create player in conference\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",playerId));
}

xmlrpc_value* DeletePlayer(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int playerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->DeletePlayer(playerId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete player from conference\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartPlaying(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int playerId;
	char *filename;
	int loop;
	xmlrpc_parse_value(env, param_array, "(iisi)", &confId, &playerId, &filename, &loop);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StartPlaying(playerId,filename,loop);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not start playback\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StopPlaying(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int playerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopPlaying(playerId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not stop play back in conference\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StartRecordingParticipant(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int playerId;
	char *filename;
	xmlrpc_parse_value(env, param_array, "(iis)", &confId, &playerId, &filename);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StartRecordingParticipant(playerId,filename);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not start playback\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* StopRecordingParticipant(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int playerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId,&playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->StopRecordingParticipant(playerId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not stop play back in conference\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* SendFPU(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId, &partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SendFPU(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not start playback\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* SetMute(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	int media;
	int isMuted;
	xmlrpc_parse_value(env, param_array, "(iiii)", &confId, &partId, &media, &isMuted);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//La borramos
	int res = conf->SetMute(partId,(MediaFrame::Type)media,isMuted);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not start playback\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* GetParticipantStatistics(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MCU *mcu = (MCU *)user_data;
	MultiConf *conf = NULL;

	 //Parseamos
	int confId;
	int partId;
	xmlrpc_parse_value(env, param_array, "(ii)", &confId, &partId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Obtenemos la referencia
	if(!mcu->GetConferenceRef(confId,&conf))
		return xmlerror(env,"Conference does not exist");

	//Get statistics
	MultiConf::ParticipantStatistics *partStats = conf->GetParticipantStatistic(partId);

	//Liberamos la referencia
	mcu->ReleaseConferenceRef(confId);

	//Salimos
	if(!partStats)
		return xmlerror(env,"Participant not found\n");

	//Create array
	xmlrpc_value* arr = xmlrpc_array_new(env);
	
	//Process result
	for (MultiConf::ParticipantStatistics::iterator it = partStats->begin(); it!=partStats->end(); ++it)
	{
		//Get media
		std::string media = it->first;
		//Get stats
		MediaStatistics stats = it->second;
		//Create array
		xmlrpc_value* val = xmlrpc_build_value(env,"(siiiiiii)",media.c_str(),stats.isReceiving,stats.isSending,stats.lostRecvPackets,stats.numRecvPackets,stats.numSendPackets,stats.totalRecvBytes,stats.totalSendBytes);
		//Add it
		xmlrpc_array_append_item(env,arr,val);
		//Release
		xmlrpc_DECREF(val);
	}

	//return
	return xmlok(env,arr);
}

xmlrpc_value* MCUEventQueueCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
        MCU *mcu = (MCU *)user_data;

	//Create the event queue
	int queueId = mcu->CreateEventQueue();

	//Si error
	if (!queueId>0)
		return xmlerror(env,"Could not create event queue");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",queueId));
}

xmlrpc_value* MCUEventQueueDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
        MCU *mcu = (MCU *)user_data;

	 //Parseamos
	int queueId;
	xmlrpc_parse_value(env, param_array, "(i)", &queueId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		xmlerror(env,"Fault occurred\n");

	//Delete event queue
	if (!mcu->DeleteEventQueue(queueId))
		return xmlerror(env,"Event queue does not exist");

	//Devolvemos el resultado
	return xmlok(env);
}

XmlHandlerCmd mcuCmdList[] =
{
	{"EventQueueCreate",MCUEventQueueCreate},
	{"EventQueueDelete",MCUEventQueueDelete},
	{"CreateConference",CreateConference},
	{"DeleteConference",DeleteConference},
	{"GetConferences",GetConferences},
	{"CreateMosaic",CreateMosaic},
	{"SetMosaicOverlayImage",SetMosaicOverlayImage},
	{"ResetMosaicOverlay",ResetMosaicOverlay},
	{"DeleteMosaic",DeleteMosaic},
	{"CreateParticipant",CreateParticipant},
	{"DeleteParticipant",DeleteParticipant},
	{"StartBroadcaster",StartBroadcaster},
	{"StopBroadcaster",StopBroadcaster},
	{"StartRecordingParticipant",StartRecordingParticipant},
	{"StopRecordingParticipant",StopRecordingParticipant},
	{"SetVideoCodec",SetVideoCodec},
	{"StartSendingVideo",StartSendingVideo},
	{"StopSendingVideo",StopSendingVideo},
	{"StartReceivingVideo",StartReceivingVideo},
	{"StopReceivingVideo",StopReceivingVideo},
	{"SetAudioCodec",SetAudioCodec},
	{"StartSendingAudio",StartSendingAudio},
	{"StopSendingAudio",StopSendingAudio},
	{"StartReceivingAudio",StartReceivingAudio},
	{"StopReceivingAudio",StopReceivingAudio},
	{"SetTextCodec",SetTextCodec},
	{"StartSendingText",StartSendingText},
	{"StopSendingText",StopSendingText},
	{"StartReceivingText",StartReceivingText},
	{"StopReceivingText",StopReceivingText},
	{"SetCompositionType",SetCompositionType},
	{"SetMosaicSlot",SetMosaicSlot},
	{"AddConferenceToken",AddConferenceToken},
	{"AddMosaicParticipant",AddMosaicParticipant},
	{"RemoveMosaicParticipant",RemoveMosaicParticipant},
	{"CreatePlayer",CreatePlayer},
	{"DeletePlayer",DeletePlayer},
	{"StartPlaying",StartPlaying},
	{"StopPlaying",StopPlaying},
	{"SendFPU",SendFPU},
	{"SetMute",SetMute},
	{"GetParticipantStatistics",GetParticipantStatistics},
	{"AddParticipantInputToken",AddParticipantInputToken},
	{"AddParticipantOutputToken",AddParticipantOutputToken},
	{"SetParticipantMosaic",SetParticipantMosaic},
	{NULL,NULL}
};
