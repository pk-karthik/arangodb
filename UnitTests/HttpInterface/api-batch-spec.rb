# coding: utf-8

require 'rspec'
require './arangodb.rb'
require './arangomultipartbody.rb'

describe ArangoDB do
  prefix = "api-batch"

  context "dealing with HTTP batch requests:" do

################################################################################
## checking disallowed methods
################################################################################
  
    context "using disallowed methods:" do

      it "checks whether GET is allowed on /_api/batch" do
        cmd = "/_api/batch"
        doc = ArangoDB.log_get("#{prefix}-get", cmd)

        doc.code.should eq(405)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(405)
      end
    
      it "checks whether HEAD is allowed on /_api/batch" do
        cmd = "/_api/batch"
        doc = ArangoDB.log_head("#{prefix}-get", cmd)

        doc.code.should eq(405)
        doc.response.body.should be_nil
      end
    
      it "checks whether DELETE is allowed on /_api/batch" do
        cmd = "/_api/batch"
        doc = ArangoDB.log_delete("#{prefix}-delete", cmd)

        doc.code.should eq(405)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(405)
      end
    
      it "checks whether PATCH is allowed on /_api/batch" do
        cmd = "/_api/batch"
        doc = ArangoDB.log_patch("#{prefix}-patch", cmd)

        doc.code.should eq(405)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(405)
      end

    end

################################################################################
## checking invalid posts
################################################################################
  
    context "checking wrong/missing content-type/boundary:" do
      it "checks missing content-type" do
        cmd = "/_api/batch"

        doc = ArangoDB.log_post("#{prefix}-post-ct-none", cmd, :body => "xx" )

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end
      
      it "checks invalid content-type (xxx)" do
        cmd = "/_api/batch"

        doc = ArangoDB.log_post("#{prefix}-post-ct-wrong-1", cmd, :body => "xx", :headers => { "Content-Type" => "xxx/xxx" } )

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end
      
      it "checks invalid content-type (json)" do
        cmd = "/_api/batch"

        doc = ArangoDB.log_post("#{prefix}-post-ct-wrong-2", cmd, :body => "xx", :headers => { "Content-Type" => "application/json" } )

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end
      
      it "checks valid content-type with missing boundary" do
        cmd = "/_api/batch"

        doc = ArangoDB.log_post("#{prefix}-post-boundary-wrong-1", cmd, :body => "xx", :headers => { "Content-Type" => "multipart/form-data" } )

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end
      
      it "checks valid content-type with unexpected boundary" do
        cmd = "/_api/batch"

        doc = ArangoDB.log_post("#{prefix}-post-boundary-wrong-2", cmd, :body => "xx", :headers => { "Content-Type" => "multipart/form-data; peng" } )

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end
      
      it "checks valid content-type with broken boundary" do
        cmd = "/_api/batch"

        doc = ArangoDB.log_post("#{prefix}-post-boundary-wrong-3", cmd, :body => "xx", :headers => { "Content-Type" => "multipart/form-data; boundary=" } )

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end
      
      it "checks valid content-type with too short boundary" do
        cmd = "/_api/batch"

        doc = ArangoDB.log_post("#{prefix}-post-boundary-wrong-4", cmd, :body => "xx", :headers => { "Content-Type" => "multipart/form-data; boundary=a" } )

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end

    end

################################################################################
## checking simple batches
################################################################################
  
    context "checking simple requests:" do
      
      it "checks an empty operation multipart message" do
        cmd = "/_api/batch"

        multipart = ArangoMultipartBody.new()

        doc = ArangoDB.log_post("#{prefix}-post-version-empty", cmd, :body => multipart.to_s, :format => :json, :headers => { "Content-Type" => "multipart/form-data; boundary=" + multipart.getBoundary })

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true) 
        doc.parsed_response['code'].should eq(400)
      end

      it "checks a multipart message with a single operation" do
        cmd = "/_api/batch"

        multipart = ArangoMultipartBody.new()

        multipart.addPart("GET", "/_api/version", { }, "")
        doc = ArangoDB.log_post("#{prefix}-post-version-one", cmd, :body => multipart.to_s, :format => :plain, :headers => { "Content-Type" => "multipart/form-data; boundary=" + multipart.getBoundary })

        doc.code.should eq(200)

        parts = multipart.getParts(multipart.getBoundary, doc.response.body)

        parts.each do|part|
          part[:status].should eq(200)
        end
      end
      
      it "checks a multipart message with a multiple operations" do
        cmd = "/_api/batch"

        multipart = ArangoMultipartBody.new()

        multipart.addPart("GET", "/_api/version", { }, "")
        multipart.addPart("GET", "/_api/version", { }, "")
        multipart.addPart("GET", "/_api/version", { }, "")
        multipart.addPart("GET", "/_api/version", { }, "")
        doc = ArangoDB.log_post("#{prefix}-post-version-mult", cmd, :body => multipart.to_s, :format => :plain, :headers => { "Content-Type" => "multipart/form-data; boundary=" + multipart.getBoundary })

        doc.code.should eq(200)

        parts = multipart.getParts(multipart.getBoundary, doc.response.body)

        parts.each do|part|
          part[:status].should eq(200)
        end
      end
      
      it "checks a multipart message with many operations" do
        cmd = "/_api/batch"

        multipart = ArangoMultipartBody.new()

        (1..128).each do
          multipart.addPart("GET", "/_api/version", { }, "")
        end
        doc = ArangoDB.log_post("#{prefix}-post-version-many", cmd, :body => multipart.to_s, :format => :plain, :headers => { "Content-Type" => "multipart/form-data; boundary=" + multipart.getBoundary })

        doc.code.should eq(200)

        parts = multipart.getParts(multipart.getBoundary, doc.response.body)

        parts.each do|part|
          part[:status].should eq(200)
        end
      end
      
      it "checks a multipart message inside a multipart message" do
        cmd = "/_api/batch"

        multipart = ArangoMultipartBody.new()
        inner = ArangoMultipartBody.new("innerBoundary")
        inner.addPart("GET", "/_api/version", { }, "")

        multipart.addPart("POST", "/_api/batch", { "Content-Type" => "multipart/form-data; boundary=innerBoundary"}, inner.to_s)
        doc = ArangoDB.log_post("#{prefix}-post-version-nested", cmd, :body => multipart.to_s, :format => :plain, :headers => { "Content-Type" => "multipart/form-data; boundary=" + multipart.getBoundary })

        doc.code.should eq(200)

        parts = multipart.getParts(multipart.getBoundary, doc.response.body)

        parts.each do|part|
          part[:status].should eq(200)
        end
      end
      
      it "checks a few multipart messages inside a multipart message" do
        cmd = "/_api/batch"

        multipart = ArangoMultipartBody.new()
        inner = ArangoMultipartBody.new("innerBoundary")
        inner.addPart("GET", "/_api/version", { }, "")
        inner.addPart("GET", "/_api/version", { }, "")
        inner.addPart("GET", "/_api/version", { }, "")
        inner.addPart("GET", "/_api/version", { }, "")
        inner.addPart("GET", "/_api/version", { }, "")
        inner.addPart("GET", "/_api/version", { }, "")

        multipart.addPart("POST", "/_api/batch", { "Content-Type" => "multipart/form-data; boundary=innerBoundary"}, inner.to_s)
        doc = ArangoDB.log_post("#{prefix}-post-version-nested", cmd, :body => multipart.to_s, :format => :plain, :headers => { "Content-Type" => "multipart/form-data; boundary=" + multipart.getBoundary })

        doc.code.should eq(200)

        parts = multipart.getParts(multipart.getBoundary, doc.response.body)

        parts.each do|part|
          part[:status].should eq(200)
        end
      end

    end

################################################################################
## checking document creation
################################################################################
    
    context "checking document creation:" do

      before do
	@cn = "UnitTestsBatch"
	ArangoDB.drop_collection(@cn)
      end

      after do
	ArangoDB.drop_collection(@cn)
      end
      
      it "checks batch document creation" do
        cmd = "/_api/batch"

        multipart = ArangoMultipartBody.new()
        multipart.addPart("POST", "/_api/collection", { }, "{\"name\":\"#{@cn}\"}")
        (1..10).each do
          multipart.addPart("POST", "/_api/document?collection=#{@cn}", { }, "{\"a\":1,\"b\":2}")
        end

        doc = ArangoDB.log_post("#{prefix}-post-documents", cmd, :body => multipart.to_s, :format => :plain, :headers => { "Content-Type" => "multipart/form-data; boundary=" + multipart.getBoundary })

        doc.code.should eq(200)

        parts = multipart.getParts(multipart.getBoundary, doc.response.body)

        partNumber = 0
        parts.each do|part|
          if partNumber == 0
            part[:status].should eq(200)
          else
            part[:status].should eq(202)
          end

          partNumber = partNumber + 1
        end
      end
    
    end
      
  end

end
